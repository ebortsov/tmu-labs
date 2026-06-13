#include "../small_vector.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <memory>
#include <numeric>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

using namespace small_vector;

enum ActionType : uint8_t {
  Allocate,
  Deallocate,
};

struct RawAllocRecord {
  ActionType type;
  void *ptr;
  size_t size;

  RawAllocRecord(ActionType type, void *ptr, size_t size) : type(type), ptr(ptr), size(size) {}
};

struct AllocRecord {
  ActionType type;
  uint64_t ptr_id;
  size_t size;

  AllocRecord(ActionType type, uint64_t ptr_id, size_t size) : type(type), ptr_id(ptr_id), size(size) {}

  bool operator==(const AllocRecord &other) const {
    return type == other.type && ptr_id == other.ptr_id && size == other.size;
  }

  friend std::ostream &operator<<(std::ostream &os, const AllocRecord &record) {
    return os << (record.type == Allocate ? "Allocate" : "Deallocate") << "(@" << record.ptr_id << "[" << record.size
              << "])";
  }
};

class AllocHistory {
public:
  explicit AllocHistory(const std::vector<RawAllocRecord> &raw_records) {
    records.reserve(raw_records.size());
    std::unordered_map<void *, uint64_t> ptr_id_map{};
    uint64_t next_id = 0;

    auto get_id = [&ptr_id_map, &next_id](void *ptr) {
      auto elem = ptr_id_map.find(ptr);
      uint64_t id;
      if (elem != ptr_id_map.end()) {
        id = elem->second;
        ptr_id_map.erase(elem);
      } else {
        id = next_id++;
        ptr_id_map[ptr] = id;
      }
      return id;
    };

    for (const auto &raw_record : raw_records) {
      uint64_t id = get_id(raw_record.ptr);
      records.emplace_back(raw_record.type, id, raw_record.size);
    }
  }

  AllocHistory(std::initializer_list<AllocRecord> records) : records(records) {}

  auto begin() const { return records.begin(); }
  auto end() const { return records.end(); }

  bool operator==(const AllocHistory &other) const { return records == other.records; }

  friend std::ostream &operator<<(std::ostream &os, const AllocHistory &history) {
    if (history.records.empty()) {
      os << "[]";
      return os;
    }
    os << "[" << history.records[0];
    for (const auto &record : history.records | std::ranges::views::drop(1)) {
      os << ", " << record;
    }
    return os << "]";
  }

private:
  std::vector<AllocRecord> records;
};

template <typename T>
class RecordingAllocator {
public:
  using value_type = T;

  RecordingAllocator() : _history(std::make_shared<std::vector<RawAllocRecord>>()) {}

  T *allocate(size_t n) {
    T *ptr = std::allocator<T>{}.allocate(n);
    _history->emplace_back(Allocate, reinterpret_cast<void *>(ptr), n);
    return ptr;
  }

  void deallocate(T *ptr, size_t n) {
    _history->emplace_back(Deallocate, reinterpret_cast<void *>(ptr), n);
    std::allocator<T>{}.deallocate(ptr, n);
  }

  AllocHistory get_history() const { return AllocHistory{*_history}; }

  void assert_no_alloc() const { ASSERT_EQ(get_history(), AllocHistory{}) << "expected no allocations"; }

  void assert_no_leaks() const {
    std::unordered_set<uint64_t> allocated_ptrs;
    for (const auto &record : get_history()) {
      if (record.type == Allocate) {
        allocated_ptrs.insert(record.ptr_id);
      } else {
        auto elem = allocated_ptrs.find(record.ptr_id);
        ASSERT_NE(elem, allocated_ptrs.end())
            << "free of non-allocated memory or double-free\nid: " << record.ptr_id << "\nhistory: " << get_history();
        allocated_ptrs.erase(record.ptr_id);
      }
    }
    if (!allocated_ptrs.empty()) {
      FAIL() << "memory leaks detected in allocations\nleaks in: " << ::testing::PrintToString(allocated_ptrs)
             << "\nhistory: " << get_history();
    }
  }

  ~RecordingAllocator() {
    if (_history.unique()) {
      assert_no_leaks();
    }
  }

private:
  std::shared_ptr<std::vector<RawAllocRecord>> _history;
};
//---------------------------------------------------------------------------
TEST(SmallVector_Internal, RecordingAllocator) {
  RecordingAllocator<uint64_t> alloc;
  {
    std::vector<uint64_t, RecordingAllocator<uint64_t>> v{alloc};
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
  }

  AllocHistory expected = {
      {Allocate, 0, 1},
      {Allocate, 1, 2},
      {Deallocate, 0, 1},
      {Allocate, 2, 4},
      {Deallocate, 1, 2},
      {Deallocate, 2, 4},
  };
  ASSERT_EQ(alloc.get_history(), expected);
}
//---------------------------------------------------------------------------
TEST(SmallVector, Empty) {
  RecordingAllocator<uint64_t> alloc;
  const SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>> vec{alloc};

  ASSERT_TRUE(vec.empty()) << "empty vector is empty";
  ASSERT_EQ(vec.size(), 0) << "empty vector has 0 elements";
  ASSERT_TRUE(vec.is_inline()) << "empty vector is inline";
  ASSERT_EQ(vec.capacity(), 16) << "empty vector has capacity equal to the inline size";
  constexpr size_t max_size = 8 * 16 + 8 * 4 + sizeof(RecordingAllocator<uint64_t>);
  ASSERT_TRUE(sizeof(SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>>) <= max_size)
      << "max size of vector is the inline memory plus three word sizes and an allocator: "
      << sizeof(SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>>) << " > " << max_size;

  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, EmptyWorksEvenIfTypeIsNotConstructible) {
  class A {
  public:
    A() = delete;
    A(const A &) = delete;
    A &operator=(const A &) = delete;
    A(A &&) = delete;
    A &operator=(A &&) = delete;
  };

  RecordingAllocator<A> alloc;
  const SmallVector<A, 16, RecordingAllocator<A>> vec{alloc};

  ASSERT_TRUE(vec.empty()) << "empty vector is empty";
  ASSERT_EQ(vec.size(), 0) << "empty vector has 0 elements";
  ASSERT_TRUE(vec.is_inline()) << "empty vector is inline";
  ASSERT_EQ(vec.capacity(), 16) << "empty vector has capacity equal to the inline size";
  constexpr size_t max_size = 16 + 8 * 4 + sizeof(RecordingAllocator<A>);
  ASSERT_TRUE(sizeof(SmallVector<A, 16, RecordingAllocator<A>>) <= max_size)
      << "max size of vector is the inline memory plus three word sizes and an allocator: "
      << sizeof(SmallVector<A, 16, RecordingAllocator<A>>) << " > " << max_size;

  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, SmallPushBack) {
  RecordingAllocator<uint64_t> alloc;
  SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>> vec{alloc};

  vec.push_back(42);
  vec.push_back(7);
  vec.push_back(13);

  const auto &vec_r = vec;

  ASSERT_FALSE(vec_r.empty());
  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec_r.size(), 3);
  ASSERT_EQ(vec_r.capacity(), 16);
  ASSERT_EQ(vec_r[0], 42);
  ASSERT_EQ(vec_r[1], 7);
  ASSERT_EQ(vec_r[2], 13);
  ASSERT_EQ(vec_r.at(0), 42);
  ASSERT_EQ(vec_r.at(1), 7);
  ASSERT_EQ(vec_r.at(2), 13);
  ASSERT_EQ(vec_r.front(), 42);
  ASSERT_EQ(vec_r.back(), 13);
  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, AtOutOfBoundsSmall) {
  RecordingAllocator<uint64_t> alloc;
  const SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>> vec{{42, 7, 13}, alloc};

  ASSERT_EQ(vec.size(), 3);
  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.at(2), 13);
  ASSERT_THROW(vec.at(3), std::out_of_range);
  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, AtOutOfBoundsLarge) {
  RecordingAllocator<uint64_t> alloc;
  const SmallVector<uint64_t, 2, RecordingAllocator<uint64_t>> vec{{42, 7, 13}, alloc};

  ASSERT_EQ(vec.size(), 3);
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.at(2), 13);
  ASSERT_THROW(vec.at(3), std::out_of_range);

  AllocHistory expected = {{Allocate, 0, 3}};
  ASSERT_EQ(expected, alloc.get_history());
}
//---------------------------------------------------------------------------
TEST(SmallVector, AllocatingPushBack) {
  RecordingAllocator<uint64_t> alloc;
  SmallVector<uint64_t, 3, RecordingAllocator<uint64_t>> vec{{42, 7, 13}, alloc};
  ASSERT_TRUE(vec.is_inline());

  vec.push_back(10);
  vec.push_back(100);
  vec.push_back(1000);

  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.size(), 6);
  ASSERT_EQ(vec.capacity(), 6);
  ASSERT_EQ(vec[3], 10);
  ASSERT_EQ(vec[4], 100);
  ASSERT_EQ(vec[5], 1000);

  vec.push_back(10000);

  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.size(), 7);
  ASSERT_EQ(vec.capacity(), 12);
  ASSERT_EQ(vec[6], 10000);

  AllocHistory expected_alloc = {
      {Allocate, 0, 6},
      {Allocate, 1, 12},
      {Deallocate, 0, 6},
  };
  ASSERT_EQ(alloc.get_history(), expected_alloc);
}
//---------------------------------------------------------------------------
TEST(SmallVector, ConstructorCountDefaultElemsSmall) {
  struct A {
    uint32_t id;
    A() : id(42) {}
  };
  RecordingAllocator<A> alloc;
  const SmallVector<A, 16, RecordingAllocator<A>> vec{8, alloc};
  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.size(), 8);
  for (auto i = 0; i < 8; i += 1) {
    ASSERT_EQ(vec[i].id, 42);
  }
  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, ConstructorCountDefaultElemsLarge) {
  struct A {
    uint32_t id;
    A() : id(42) {}
    A(const A &) = delete;
    A(A &&) = delete;
    A &operator=(const A &) = delete;
    A &operator=(A &&) = delete;
  };
  RecordingAllocator<A> alloc;
  const SmallVector<A, 4, RecordingAllocator<A>> vec{16, alloc};
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.size(), 16);
  ASSERT_EQ(vec.capacity(), 16);
  for (auto i = 0; i < 16; i += 1) {
    ASSERT_EQ(vec[i].id, 42);
  }
  AllocHistory expected = {{Allocate, 0, 16}};
  ASSERT_EQ(alloc.get_history(), expected);
}
//---------------------------------------------------------------------------
TEST(SmallVector, ConstructorCopyValueSmall) {
  struct A {
    uint32_t id;
    mutable uint32_t copies;
    A() : id(42), copies(0) {}
    A(const A &other) : id(other.id + other.copies + 1), copies(0) { other.copies += 1; }
    A &operator=(const A &) = delete;
  };
  RecordingAllocator<A> alloc;
  const SmallVector<A, 16, RecordingAllocator<A>> vec{8, A{}, alloc};
  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.size(), 8);
  for (auto i = 0; i < 8; i += 1) {
    ASSERT_EQ(vec[i].id, 42 + i + 1);
  }
  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, ConstructorCopyValueLarge) {
  struct A {
    uint32_t id;
    mutable uint32_t copies;
    A() : id(42), copies(0) {}
    A(const A &other) : id(other.id + other.copies + 1), copies(0) { other.copies += 1; }
    A &operator=(const A &) = delete;
  };
  RecordingAllocator<A> alloc;
  const SmallVector<A, 4, RecordingAllocator<A>> vec{16, A{}, alloc};
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.size(), 16);
  for (auto i = 0; i < 16; i += 1) {
    ASSERT_EQ(vec[i].id, 42 + i + 1);
  }
  AllocHistory expected = {{Allocate, 0, 16}};
  ASSERT_EQ(alloc.get_history(), expected);
}
//---------------------------------------------------------------------------
TEST(SmallVector, CopyConstructorSmall) {
  struct A {
    uint32_t id;
    explicit A(uint32_t id) : id(id) {}
    A(const A &other) = default;
    A &operator=(const A &) = delete;
    A(A &&) = delete;
    A &operator=(A &&) = delete;
    bool operator==(const A &other) const { return id == other.id; }
  };
  using Vec = SmallVector<A, 16, RecordingAllocator<A>>;
  RecordingAllocator<A> alloc;
  Vec vec{alloc};
  for (uint32_t i = 0; i < 16; i += 1) {
    vec.emplace_back(i);
  }
  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.size(), 16);
  Vec vec2 = vec;

  ASSERT_TRUE(vec2.is_inline());
  ASSERT_EQ(vec.size(), vec2.size());
  ASSERT_EQ(vec.capacity(), vec2.capacity());
  ASSERT_TRUE(vec == vec2) << "copied vectors should be equal";
  Vec::Iterator it1, it2;
  for (it1 = vec.begin(), it2 = vec2.begin(); it1 != vec.end(); ++it1, ++it2) {
    ASSERT_EQ(it1->id, it2->id);
  }
  ASSERT_EQ(it2, vec2.end());

  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, CopyConstructorLarge) {
  struct A {
    uint32_t id;
    explicit A(uint32_t id) : id(id) {}
    A(const A &other) = default;
    A &operator=(const A &) = delete;
    A(A &&) = delete;
    A &operator=(A &&) = delete;
    bool operator==(const A &other) const { return id == other.id; }
  };
  using Vec = SmallVector<A, 16, RecordingAllocator<A>>;
  RecordingAllocator<A> alloc;
  Vec vec = Vec::with_capacity(32, alloc);
  for (uint32_t i = 0; i < 32; i += 1) {
    vec.emplace_back(i);
  }
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.size(), 32);
  Vec vec2 = vec;

  ASSERT_FALSE(vec2.is_inline());
  ASSERT_EQ(vec.size(), vec2.size());
  ASSERT_EQ(vec.capacity(), vec2.capacity());
  ASSERT_TRUE(vec == vec2) << "copied vectors should be equal";
  Vec::Iterator it1, it2;
  for (it1 = vec.begin(), it2 = vec2.begin(); it1 != vec.end(); ++it1, ++it2) {
    ASSERT_EQ(it1->id, it2->id);
  }
  ASSERT_EQ(it2, vec2.end());

  AllocHistory expected = {{Allocate, 0, 32}, {Allocate, 1, 32}};
  ASSERT_EQ(expected, alloc.get_history()) << "note that the copy constructor should only allocated once";
}
//---------------------------------------------------------------------------
TEST(SmallVector, MoveConstructorSmallWithMove) {
  struct A {
    uint32_t id;
    uint32_t &instances;
    explicit A(uint32_t id, uint32_t &instances) : id(id), instances(instances) { ++instances; }
    A(const A &other) = delete;
    A &operator=(const A &) = delete;
    A(A &&other) noexcept : id(other.id), instances(other.instances) { ++instances; }
    A &operator=(A &&) = delete;
    bool operator==(const A &other) const { return id == other.id; }
    ~A() { instances -= 1; }
  };
  uint32_t instances = 0;
  using Vec = SmallVector<A, 16, RecordingAllocator<A>>;
  {
    RecordingAllocator<A> alloc;
    Vec vec{alloc};
    for (uint32_t i = 0; i < 16; i += 1) {
      vec.emplace_back(i, instances);
    }
    ASSERT_TRUE(vec.is_inline());
    ASSERT_EQ(16, vec.size());
    Vec vec2 = std::move(vec);

    ASSERT_TRUE(vec2.is_inline());
    ASSERT_EQ(16, vec2.size());
    ASSERT_EQ(16, instances);
    for (uint32_t i = 0; i < 16; i += 1) {
      ASSERT_EQ(i, vec2[i].id);
    }

    alloc.assert_no_alloc();
  }
  ASSERT_EQ(instances, 0);
}
//---------------------------------------------------------------------------
TEST(SmallVector, MoveConstructorSmallWithCopy) {
  struct A {
    uint32_t id;
    uint32_t &instances;
    explicit A(uint32_t id, uint32_t &instances) : id(id), instances(instances) { ++instances; }
    A(const A &other) : id(other.id), instances(other.instances) { ++instances; }
    A &operator=(const A &) = delete;
    A(A &&other) = delete;
    A &operator=(A &&) = delete;
    bool operator==(const A &other) const { return id == other.id; }
    ~A() { instances -= 1; }
  };
  uint32_t instances = 0;
  using Vec = SmallVector<A, 16, RecordingAllocator<A>>;
  {
    RecordingAllocator<A> alloc;
    Vec vec{alloc};
    for (uint32_t i = 0; i < 16; i += 1) {
      vec.emplace_back(i, instances);
    }
    ASSERT_TRUE(vec.is_inline());
    ASSERT_EQ(16, vec.size());
    Vec vec2 = std::move(vec);

    ASSERT_TRUE(vec2.is_inline());
    ASSERT_EQ(16, vec2.size());
    ASSERT_EQ(16, instances);
    for (uint32_t i = 0; i < 16; i += 1) {
      ASSERT_EQ(i, vec2[i].id);
    }

    alloc.assert_no_alloc();
  }
  ASSERT_EQ(instances, 0);
}
//---------------------------------------------------------------------------
TEST(SmallVector, MoveConstructorLarge) {
  struct A {
    uint32_t id;
    uint32_t &instances;
    explicit A(uint32_t id, uint32_t &instances) : id(id), instances(instances) { ++instances; }
    A(const A &other) = delete;
    A &operator=(const A &) = delete;
    A(A &&other) noexcept : id(other.id), instances(other.instances) { ++instances; }
    A &operator=(A &&) = delete;
    bool operator==(const A &other) const { return id == other.id; }
    ~A() { instances -= 1; }
  };
  uint32_t instances = 0;
  using Vec = SmallVector<A, 16, RecordingAllocator<A>>;
  {
    RecordingAllocator<A> alloc;
    Vec vec = Vec::with_capacity(32, alloc);
    for (uint32_t i = 0; i < 32; i += 1) {
      vec.emplace_back(i, instances);
      ASSERT_EQ(i, vec.back().id);
    }
    ASSERT_FALSE(vec.is_inline());
    ASSERT_EQ(32, vec.size());
    Vec vec2 = std::move(vec);

    ASSERT_FALSE(vec2.is_inline());
    ASSERT_EQ(32, vec2.size());
    ASSERT_EQ(32, instances);
    for (uint32_t i = 0; i < 32; i += 1) {
      ASSERT_EQ(i, vec2[i].id);
    }

    AllocHistory expected = {{Allocate, 0, 32}};
    ASSERT_EQ(expected, alloc.get_history()) << "note that the move constructor should not allocate";
  }
  ASSERT_EQ(0, instances);
}
//---------------------------------------------------------------------------
TEST(SmallVector, MoveConstructorHeapAllocatedButSmall) {
  struct A {
    uint32_t id;
    uint32_t &instances;
    explicit A(uint32_t id, uint32_t &instances) : id(id), instances(instances) { ++instances; }
    A(const A &other) = delete;
    A &operator=(const A &) = delete;
    A(A &&other) noexcept : id(other.id), instances(other.instances) { ++instances; }
    A &operator=(A &&) = delete;
    bool operator==(const A &other) const { return id == other.id; }
    ~A() { instances -= 1; }
  };
  uint32_t instances = 0;
  using Vec = SmallVector<A, 16, RecordingAllocator<A>>;
  {
    RecordingAllocator<A> alloc;
    Vec vec = Vec::with_capacity(32, alloc);
    for (uint32_t i = 0; i < 32; i += 1) {
      vec.emplace_back(i, instances);
      ASSERT_EQ(i, vec.back().id);
    }
    ASSERT_FALSE(vec.is_inline());
    ASSERT_EQ(32, vec.size());
    for (uint32_t i = 0; i < 24; i += 1) {
      vec.pop_back();
    }
    ASSERT_FALSE(vec.is_inline());
    ASSERT_EQ(8, vec.size());
    ASSERT_EQ(8, instances);
    ASSERT_FALSE(vec.is_inline());
    Vec vec2 = std::move(vec);

    ASSERT_FALSE(vec2.is_inline());
    ASSERT_EQ(8, vec2.size());
    ASSERT_EQ(8, instances);
    for (uint32_t i = 0; i < 8; i += 1) {
      ASSERT_EQ(i, vec2[i].id);
    }

    AllocHistory expected = {{Allocate, 0, 32}};
    ASSERT_EQ(expected, alloc.get_history()) << "note that the move constructor should not allocate";
  }
  ASSERT_EQ(0, instances);
}
//---------------------------------------------------------------------------
TEST(SmallVector, PushBackMovesIfItCan) {
  struct A {
    uint32_t id = 42;
    mutable bool moved = false;
    mutable bool copied = false;
    A() = default;
    A(const A &other) : id(other.id) { other.copied = true; }
    A &operator=(const A &) = delete;
    A(A &&other) noexcept : id(other.id) { other.moved = true; }
    A &operator=(const A &&) = delete;
  };
  RecordingAllocator<A> alloc;
  A a;
  SmallVector<A, 4, RecordingAllocator<A>> vec{};
  vec.push_back(std::move(a));

  ASSERT_EQ(vec.size(), 1);
  ASSERT_EQ(vec[0].id, 42);
  ASSERT_TRUE(a.moved);
  ASSERT_FALSE(a.copied);
  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, PushBackCopiesIfItMust) {
  struct A {
    uint32_t id = 42;
    mutable bool copied = false;
    A() = default;
    A(const A &other) : id(other.id) { other.copied = true; }
    A &operator=(const A &) = delete;
    A(A &&other) = delete;
    A &operator=(const A &&) = delete;
  };
  RecordingAllocator<A> alloc;
  A a;
  SmallVector<A, 4, RecordingAllocator<A>> vec{};
  vec.push_back(a);

  ASSERT_EQ(vec.size(), 1);
  ASSERT_EQ(vec[0].id, 42);
  ASSERT_TRUE(a.copied);
  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, EmplaceBackSmall) {
  struct A {
    uint32_t a, b;
    A(uint32_t a, uint32_t b) : a(a), b(b) {}
    A(const A &other) = delete;
    A &operator=(const A &) = delete;
    A(A &&other) noexcept : a(0), b(0) { other.a = other.b = 0; };
    A &operator=(const A &&) = delete;
  };
  RecordingAllocator<A> alloc;
  SmallVector<A, 4, RecordingAllocator<A>> vec{};

  vec.emplace_back(1, 2);
  vec.emplace_back(37, 42);
  vec.emplace_back(100, 1000);
  vec.emplace_back(21, 37);

  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.size(), 4);
  ASSERT_EQ(vec[0].a, 1);
  ASSERT_EQ(vec[0].b, 2);
  ASSERT_EQ(vec[1].a, 37);
  ASSERT_EQ(vec[1].b, 42);
  ASSERT_EQ(vec[2].a, 100);
  ASSERT_EQ(vec[2].b, 1000);
  ASSERT_EQ(vec[3].a, 21);
  ASSERT_EQ(vec[3].b, 37);
  alloc.assert_no_alloc();
}

TEST(SmallVector, WithCapacitySmall) {
  RecordingAllocator<uint64_t> alloc;
  const auto vec = SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>>::with_capacity(12, alloc);
  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.capacity(), 16);
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());

  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, WithCapacityLarge) {
  RecordingAllocator<uint64_t> alloc;
  auto vec = SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>>::with_capacity(37, alloc);
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.capacity(), 37);
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());

  AllocHistory expected = {{Allocate, 0, 37}};
  ASSERT_EQ(expected, alloc.get_history());

  for (size_t i = 0; i < 37; ++i) {
    vec.push_back(i);
  }
  ASSERT_EQ(expected, alloc.get_history());
  vec.push_back(2137);
  AllocHistory expected2 = {{Allocate, 0, 37}, {Allocate, 1, 74}, {Deallocate, 0, 37}};
  ASSERT_EQ(expected2, alloc.get_history());
}
//---------------------------------------------------------------------------
TEST(SmallVector, ReserveSmall) {
  RecordingAllocator<uint64_t> alloc;
  SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>> vec{};
  vec.reserve(12);
  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.capacity(), 16);
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());

  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, ReserveLarge) {
  RecordingAllocator<uint64_t> alloc;
  SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>> vec{alloc};
  vec.reserve(37);
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.capacity(), 37);
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());

  AllocHistory expected = {{Allocate, 0, 37}};
  ASSERT_EQ(expected, alloc.get_history());

  for (size_t i = 0; i < 37; ++i) {
    vec.push_back(i);
  }
  ASSERT_EQ(expected, alloc.get_history());
  vec.push_back(2137);
  ASSERT_EQ(38, vec.size());
  ASSERT_EQ(74, vec.capacity());
  AllocHistory expected2 = {{Allocate, 0, 37}, {Allocate, 1, 74}, {Deallocate, 0, 37}};
  ASSERT_EQ(expected2, alloc.get_history());

  vec.reserve(50);
  ASSERT_EQ(74, vec.capacity());
  ASSERT_EQ(expected2, alloc.get_history());
  vec.reserve(75);
  ASSERT_EQ(75, vec.capacity());
  AllocHistory expected3 = {
      {Allocate, 0, 37}, {Allocate, 1, 74}, {Deallocate, 0, 37}, {Allocate, 2, 75}, {Deallocate, 1, 74}};
  ASSERT_EQ(expected3, alloc.get_history());
}
//---------------------------------------------------------------------------
TEST(SmallVector, ReserveBelowCurrentIsNoOp) {
  RecordingAllocator<uint64_t> alloc;
  auto vec = SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>>::with_capacity(32, alloc);
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.capacity(), 32);
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());
  vec.reserve(2);
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.capacity(), 32);
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());

  AllocHistory expected = {{Allocate, 0, 32}};
  ASSERT_EQ(expected, alloc.get_history());
}
//---------------------------------------------------------------------------
TEST(SmallVector, ShrinkToFitSmall) {
  RecordingAllocator<uint64_t> alloc;
  SmallVector<uint64_t, 16, RecordingAllocator<uint64_t>> vec{alloc};

  for (uint64_t i = 0; i < 8; i += 1) {
    vec.push_back(i);
  }
  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(8, vec.size());
  ASSERT_EQ(16, vec.capacity());

  vec.shrink_to_fit();

  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(8, vec.size());
  ASSERT_EQ(16, vec.capacity());

  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, ShrinkToFitLarge) {
  RecordingAllocator<uint64_t> alloc;
  SmallVector<uint64_t, 4, RecordingAllocator<uint64_t>> vec{alloc};

  for (uint64_t i = 0; i < 20; i += 1) {
    vec.push_back(i);
  }
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(20, vec.size());
  ASSERT_EQ(32, vec.capacity());

  vec.shrink_to_fit();

  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(20, vec.size());
  ASSERT_EQ(20, vec.capacity());

  AllocHistory expected = {{Allocate, 0, 8}, {Allocate, 1, 16}, {Deallocate, 0, 8}, {Allocate, 2, 32},
      {Deallocate, 1, 16}, {Allocate, 3, 20}, {Deallocate, 2, 32}};
  ASSERT_EQ(expected, alloc.get_history());
}
//---------------------------------------------------------------------------
TEST(SmallVector, ShrinkToFitLargeToSmall) {
  RecordingAllocator<uint64_t> alloc;
  SmallVector<uint64_t, 4, RecordingAllocator<uint64_t>> vec{alloc};

  for (uint64_t i = 0; i < 5; i += 1) {
    vec.push_back(i);
  }
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(5, vec.size());
  ASSERT_EQ(8, vec.capacity());

  vec.pop_back();
  ASSERT_FALSE(vec.is_inline());
  vec.shrink_to_fit();

  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());

  AllocHistory expected = {{Allocate, 0, 8}, {Deallocate, 0, 8}};
  ASSERT_EQ(expected, alloc.get_history());
}
//---------------------------------------------------------------------------
TEST(SmallVector, ClearSmall) {
  struct A {
    uint32_t &instances;
    explicit A(uint32_t &instances) : instances(instances) { instances += 1; }
    A(const A &other) : instances(other.instances) { instances += 1; }
    A &operator=(const A &other) = delete;

    ~A() { instances -= 1; }
  };
  uint32_t active_instances = 0;
  RecordingAllocator<A> alloc;
  SmallVector<A, 4, RecordingAllocator<A>> vec{
      {A{active_instances}, A{active_instances}, A{active_instances}, A{active_instances}}, alloc};

  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.size(), 4);
  ASSERT_EQ(active_instances, 4);
  vec.clear();
  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(active_instances, 0);
  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, ClearLarge) {
  struct A {
    uint32_t &instances;
    explicit A(uint32_t &instances) : instances(instances) { instances += 1; }
    A(const A &other) : instances(other.instances) { instances += 1; }
    A &operator=(const A &other) = delete;

    ~A() { instances -= 1; }
  };
  uint32_t active_instances = 0;
  RecordingAllocator<A> alloc;
  SmallVector<A, 4, RecordingAllocator<A>> vec{64, A{active_instances}, alloc};

  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.size(), 64);
  ASSERT_EQ(vec.capacity(), 64);
  ASSERT_EQ(active_instances, 64);
  vec.clear();
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(vec.capacity(), 64);
  ASSERT_EQ(active_instances, 0);
  AllocHistory expected = {{Allocate, 0, 64}};
  ASSERT_EQ(expected, alloc.get_history()) << "note that clear does not deallocate";
}
//---------------------------------------------------------------------------
TEST(SmallVector, ClearLargeAndShrinkToFit) {
  struct A {
    uint32_t &instances;
    explicit A(uint32_t &instances) : instances(instances) { instances += 1; }
    A(const A &other) : instances(other.instances) { instances += 1; }
    A &operator=(const A &other) = delete;

    ~A() { instances -= 1; }
  };
  uint32_t active_instances = 0;
  RecordingAllocator<A> alloc;
  SmallVector<A, 4, RecordingAllocator<A>> vec{64, A{active_instances}, alloc};

  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.size(), 64);
  ASSERT_EQ(vec.capacity(), 64);
  ASSERT_EQ(active_instances, 64);
  vec.clear();
  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(vec.capacity(), 64);
  ASSERT_EQ(active_instances, 0);
  AllocHistory expected1 = {{Allocate, 0, 64}};
  ASSERT_EQ(expected1, alloc.get_history()) << "note that clear does not deallocate";

  vec.shrink_to_fit();
  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(vec.capacity(), 4);
  AllocHistory expected2 = {{Allocate, 0, 64}, {Deallocate, 0, 64}};
  ASSERT_EQ(expected2, alloc.get_history());
}
//---------------------------------------------------------------------------
TEST(SmallVector, SmallPopBack) {
  struct A {
    uint32_t &instances;
    explicit A(uint32_t &instances) : instances(instances) { instances += 1; }
    A(const A &other) : instances(other.instances) { instances += 1; }
    A &operator=(const A &other) = delete;

    ~A() { instances -= 1; }
  };
  uint32_t active_instances = 0;
  RecordingAllocator<A> alloc;
  SmallVector<A, 4, RecordingAllocator<A>> vec{
      {A{active_instances}, A{active_instances}, A{active_instances}, A{active_instances}}, alloc};

  for (int i = 4; i > 0; i -= 1) {
    ASSERT_TRUE(vec.is_inline());
    ASSERT_EQ(vec.size(), i);
    ASSERT_EQ(active_instances, i);
    vec.pop_back();
  }

  ASSERT_TRUE(vec.is_inline());
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(active_instances, 0);
  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, LargePopBack) {
  struct A {
    uint32_t &instances;
    explicit A(uint32_t &instances) : instances(instances) { instances += 1; }
    A(const A &other) : instances(other.instances) { instances += 1; }
    A &operator=(const A &other) = delete;

    ~A() { instances -= 1; }
  };
  uint32_t active_instances = 0;
  RecordingAllocator<A> alloc;
  SmallVector<A, 4, RecordingAllocator<A>> vec{64, A{active_instances}, alloc};

  for (int i = 64; i > 0; i -= 1) {
    ASSERT_FALSE(vec.is_inline());
    ASSERT_EQ(vec.size(), i);
    ASSERT_EQ(active_instances, i);
    vec.pop_back();
  }

  ASSERT_FALSE(vec.is_inline());
  ASSERT_EQ(vec.size(), 0);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(active_instances, 0);
  AllocHistory expected = {{Allocate, 0, 64}};
  ASSERT_EQ(expected, alloc.get_history()) << "note that pop_back does not deallocate";
}
//---------------------------------------------------------------------------
TEST(SmallVector, ConstIterTraits) {
  using Vec = SmallVector<uint32_t, 4, RecordingAllocator<uint32_t>>;
  static_assert(std::input_iterator<Vec::ConstIterator>);
  static_assert(std::forward_iterator<Vec::ConstIterator>);
  static_assert(std::bidirectional_iterator<Vec::ConstIterator>);
  static_assert(std::random_access_iterator<Vec::ConstIterator>);
  static_assert(std::contiguous_iterator<Vec::ConstIterator>);
}
//---------------------------------------------------------------------------
TEST(SmallVector, IterTraits) {
  using Vec = SmallVector<uint32_t, 4, RecordingAllocator<uint32_t>>;
  static_assert(std::input_iterator<Vec::Iterator>);
  static_assert(std::output_iterator<Vec::Iterator, uint32_t>);
  static_assert(std::forward_iterator<Vec::Iterator>);
  static_assert(std::bidirectional_iterator<Vec::Iterator>);
  static_assert(std::random_access_iterator<Vec::Iterator>);
  static_assert(std::contiguous_iterator<Vec::Iterator>);
}
//---------------------------------------------------------------------------
TEST(SmallVector, IterIsContiguousSmall) {
  using Vec = SmallVector<uint32_t, 16, RecordingAllocator<uint32_t>>;
  Vec vec{16};

  uint32_t *ptr = vec.data();
  Vec::Iterator iter = vec.begin();

  for (size_t i = 0; i < 16; i += 1) {
    ASSERT_EQ(ptr + i, &*(iter + i));
  }
}
//---------------------------------------------------------------------------
TEST(SmallVector, IterIsContiguousLarge) {
  using Vec = SmallVector<uint32_t, 4, RecordingAllocator<uint32_t>>;
  Vec vec{16};

  uint32_t *ptr = vec.data();
  Vec::Iterator iter = vec.begin();

  for (size_t i = 0; i < 16; i += 1) {
    ASSERT_EQ(ptr + i, &*(iter + i));
  }
}
//---------------------------------------------------------------------------
TEST(SmallVector, IterSmall) {
  using Vec = SmallVector<uint32_t, 64, RecordingAllocator<uint32_t>>;
  RecordingAllocator<uint32_t> alloc;
  Vec vec{64, alloc};
  std::ranges::iota(vec.begin(), vec.end(), 0);

  const Vec &vec_r = vec;
  Vec::ConstIterator iter = vec_r.begin();
  uint32_t i;
  for (i = 0; iter != vec_r.end(); ++iter, ++i) {
    ASSERT_EQ(*iter, i);
  }
  ASSERT_EQ(i, 64);
  alloc.assert_no_alloc();
}
//---------------------------------------------------------------------------
TEST(SmallVector, IterLarge) {
  using Vec = SmallVector<uint32_t, 16, RecordingAllocator<uint32_t>>;
  RecordingAllocator<uint32_t> alloc;
  Vec vec{64, alloc};
  std::ranges::iota(vec.begin(), vec.end(), 0);

  const Vec &vec_r = vec;
  Vec::ConstIterator iter = vec_r.begin();
  uint32_t i;
  for (i = 0; iter != vec_r.end(); ++iter, ++i) {
    ASSERT_EQ(*iter, i);
  }
  ASSERT_EQ(i, 64);
  AllocHistory expected = {{Allocate, 0, 64}};
  ASSERT_EQ(expected, alloc.get_history());
}
//---------------------------------------------------------------------------
TEST(SmallVector, IterLargeWriting) {
  using Vec = SmallVector<uint32_t, 16, RecordingAllocator<uint32_t>>;
  RecordingAllocator<uint32_t> alloc;
  Vec vec{64, 42, alloc};

  Vec::Iterator iter = vec.begin();
  ASSERT_EQ(vec.end() - iter, 64);
  ASSERT_TRUE(iter < vec.end());
  uint32_t i;
  for (i = 0; iter != vec.end(); ++iter, ++i) {
    *iter += i;
  }
  ASSERT_EQ(i, 64);
  for (i = 0, iter = vec.begin(); iter != vec.end(); ++iter, ++i) {
    ASSERT_EQ(*iter, 42 + i);
  }
  AllocHistory expected = {{Allocate, 0, 64}};
  ASSERT_EQ(expected, alloc.get_history());
}
//---------------------------------------------------------------------------
TEST(SmallVector, Equality) {
  using Vec = SmallVector<uint32_t, 2>;
  Vec empty{};
  Vec v123{1, 2, 3};
  Vec v21{2, 1};
  Vec v1234{1, 2, 3, 4};
  Vec v123also{1, 2, 3};

  ASSERT_TRUE(empty == Vec{}); // NOLINT
  ASSERT_FALSE(v123 == v1234);
  ASSERT_TRUE(v123 != v1234);
  ASSERT_TRUE(v1234 != v123);
  ASSERT_TRUE(v123 != v21);
  ASSERT_TRUE(v123 == v123also);
  ASSERT_TRUE(v123also == v123);
  ASSERT_TRUE(v123 == v123);
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
