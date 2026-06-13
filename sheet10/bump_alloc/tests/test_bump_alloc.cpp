#include "../bump_alloc.hpp"
#include <gtest/gtest.h>
#include <random>

using namespace bump_alloc;

//---------------------------------------------------------------------------
// Pointer arithmetic tests
// These tests don't actually use any of the memory, they just perform allocations
// to make sure the internal bump ptr arithmetic checks out. As such the values given
// as start and end aren't actually usable memory.
//---------------------------------------------------------------------------
TEST(BumpAllocatorPtrArithmetic, Construction) {
  auto start = reinterpret_cast<std::byte *>(0x10000);
  auto end = reinterpret_cast<std::byte *>(0x20000);

  RawBumpAllocator raw_alloc{start, 0x10000};
  BumpAllocator<std::byte> alloc{&raw_alloc};

  ASSERT_EQ(start, &*raw_alloc.allocation_span().begin());
  ASSERT_EQ(end, &*raw_alloc.allocation_span().end());
  ASSERT_EQ(0x10000, alloc.max_size());
  ASSERT_EQ(0x10000, alloc.remaining_size());
  ASSERT_EQ(0, raw_alloc.allocated_bytes());
  ASSERT_EQ(alloc, alloc) << "equality must be reflexive";
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorPtrArithmetic, ConstructionMaxSize) {
  auto start = reinterpret_cast<std::byte *>(0x10001);
  auto end = reinterpret_cast<std::byte *>(0x1FFFF);
  size_t total_size = end - start;

  RawBumpAllocator raw_alloc{start, total_size};
  BumpAllocator<uint64_t> alloc{&raw_alloc};

  ASSERT_EQ(start, &*raw_alloc.allocation_span().begin());
  ASSERT_EQ(end, &*raw_alloc.allocation_span().end());
  ASSERT_EQ(8190, alloc.max_size()) << "The allocator has 0xFFFE bytes to allocate from, which is max of 8191 uint64_t "
                                       "allocations. However, due to alignment, the actual max is one less.";
  ASSERT_EQ(8190, alloc.remaining_size());
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorPtrArithmetic, AllocatesAlignedAndDownward) {
  auto start = reinterpret_cast<std::byte *>(0x10001);
  auto end = reinterpret_cast<std::byte *>(0x1FFFF);
  size_t total_size = end - start;

  RawBumpAllocator raw_alloc{start, total_size};
  BumpAllocator<uint64_t> alloc{&raw_alloc};

  uint64_t *ptr = alloc.allocate(16);

  ASSERT_EQ(0x1FF78, reinterpret_cast<size_t>(ptr)) << "this is the first possible downwards-bumping allocation that is"
                                                       "properly aligned";
  ASSERT_EQ(16 * 8 + 7, raw_alloc.allocated_bytes()) << "allocated_size must include space wasted for alignment";
  ASSERT_EQ(8174, alloc.remaining_size());

  uint64_t *ptr2 = alloc.allocate(7);
  ASSERT_EQ(0x1FF40, reinterpret_cast<size_t>(ptr2));
  ASSERT_EQ(23 * 8 + 7, raw_alloc.allocated_bytes());
  ASSERT_EQ(8167, alloc.remaining_size());
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorPtrArithmetic, MixedAllocations) {
  auto start = reinterpret_cast<std::byte *>(0x10000);
  auto end = reinterpret_cast<std::byte *>(0x20000);
  size_t total_size = end - start;
  RawBumpAllocator raw_alloc{start, total_size};

  BumpAllocator<uint8_t> alloc1{&raw_alloc};
  BumpAllocator<uint16_t> alloc2{&raw_alloc};
  BumpAllocator<uint32_t> alloc4{&raw_alloc};
  BumpAllocator<uint64_t> alloc8{&raw_alloc};

  uint8_t *ptr1 = alloc1.allocate(100);
  size_t size1 = raw_alloc.allocated_bytes();
  uint64_t *ptr2 = alloc8.allocate(16);
  size_t size2 = raw_alloc.allocated_bytes();
  uint8_t *ptr3 = alloc1.allocate(9);
  size_t size3 = raw_alloc.allocated_bytes();
  uint32_t *ptr4 = alloc4.allocate(100);
  size_t size4 = raw_alloc.allocated_bytes();
  uint16_t *ptr5 = alloc2.allocate(100);
  size_t size5 = raw_alloc.allocated_bytes();
  uint64_t *ptr6 = alloc8.allocate(100);
  size_t size6 = raw_alloc.allocated_bytes();

  ASSERT_EQ(0x1FF9C, reinterpret_cast<size_t>(ptr1));
  ASSERT_EQ(100, size1);
  ASSERT_EQ(0x1FF18, reinterpret_cast<size_t>(ptr2));
  ASSERT_EQ(232, size2);
  ASSERT_EQ(0x1FF0F, reinterpret_cast<size_t>(ptr3));
  ASSERT_EQ(241, size3);
  ASSERT_EQ(0x1FD7C, reinterpret_cast<size_t>(ptr4));
  ASSERT_EQ(644, size4);
  ASSERT_EQ(0x1FCB4, reinterpret_cast<size_t>(ptr5));
  ASSERT_EQ(844, size5);
  ASSERT_EQ(0x1F990, reinterpret_cast<size_t>(ptr6));
  ASSERT_EQ(1648, size6);

  raw_alloc.clear();
  ASSERT_EQ(0, raw_alloc.allocated_bytes());
  ASSERT_EQ(0x10000, alloc1.remaining_size());
  ASSERT_EQ(0x08000, alloc2.remaining_size());
  ASSERT_EQ(0x04000, alloc4.remaining_size());
  ASSERT_EQ(0x02000, alloc8.remaining_size());
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorPtrArithmetic, CopyingPreservesAllocations) {
  auto start = reinterpret_cast<std::byte *>(0x10001);
  auto end = reinterpret_cast<std::byte *>(0x1FFFF);
  size_t total_size = end - start;

  RawBumpAllocator raw_alloc{start, total_size};
  BumpAllocator<uint64_t> alloc{&raw_alloc};

  uint64_t *ptr = alloc.allocate(16);
  ASSERT_EQ(0x1FF78, reinterpret_cast<size_t>(ptr)) << "this is the first possible downwards-bumping allocation that is"
                                                       "properly aligned";
  ASSERT_EQ(16 * 8 + 7, raw_alloc.allocated_bytes()) << "allocated_size must include space wasted for alignment";

  auto alloc_copy = alloc;
  ASSERT_EQ(alloc, alloc_copy);
  uint64_t *ptr2 = alloc_copy.allocate(7);
  ASSERT_EQ(0x1FF40, reinterpret_cast<size_t>(ptr2));
  ASSERT_EQ(23 * 8 + 7, raw_alloc.allocated_bytes());

  uint64_t *ptr3 = alloc.allocate(7);
  ASSERT_EQ(0x1FF08, reinterpret_cast<size_t>(ptr3));
  ASSERT_EQ(30 * 8 + 7, raw_alloc.allocated_bytes());
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorPtrArithmetic, AllocatingOverCapacityThrowsBadAlloc) {
  auto start = reinterpret_cast<std::byte *>(0x10000);
  auto end = reinterpret_cast<std::byte *>(0x20000);
  size_t total_size = end - start;

  RawBumpAllocator raw_alloc{start, total_size};
  BumpAllocator<uint64_t> alloc{&raw_alloc};

  ASSERT_THROW(alloc.allocate(total_size / 8 + 1), std::bad_alloc);
  ASSERT_EQ(0, raw_alloc.allocated_bytes());

  uint64_t *ptr = alloc.allocate(total_size / 8);
  ASSERT_EQ(0x10000, reinterpret_cast<size_t>(ptr));
  ASSERT_EQ(0x10000, raw_alloc.allocated_bytes());
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorPtrArithmetic, AllocatingOverCapacityWithOverflowThrowsBadAlloc) {
  auto start = reinterpret_cast<std::byte *>(0x10000);
  auto end = reinterpret_cast<std::byte *>(0x20000);
  size_t total_size = end - start;

  RawBumpAllocator raw_alloc{start, total_size};
  BumpAllocator<uint64_t> alloc{&raw_alloc};

  ASSERT_THROW(alloc.allocate(0x1000000000000), std::bad_alloc);
  ASSERT_EQ(0, raw_alloc.allocated_bytes());

  uint64_t *ptr = alloc.allocate(total_size / 8);
  ASSERT_EQ(0x10000, reinterpret_cast<size_t>(ptr));
  ASSERT_EQ(0x10000, raw_alloc.allocated_bytes());
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorPtrArithmetic, AllocatingOverCapacityWithOverflowThrowsBadAlloc2) {
  auto start = reinterpret_cast<std::byte *>(0x10000);
  auto end = reinterpret_cast<std::byte *>(0x20000);
  size_t total_size = end - start;

  RawBumpAllocator raw_alloc{start, total_size};
  BumpAllocator<uint64_t> alloc{&raw_alloc};
  size_t allocation_request = SIZE_MAX / 8 - 16;

  ASSERT_THROW(alloc.allocate(allocation_request), std::bad_alloc);
  ASSERT_EQ(0, raw_alloc.allocated_bytes());

  uint64_t *ptr = alloc.allocate(total_size / 8);
  ASSERT_EQ(0x10000, reinterpret_cast<size_t>(ptr));
  ASSERT_EQ(0x10000, raw_alloc.allocated_bytes());
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorPtrArithmetic, AllocatingOverCapacityDueToAlignmentThrowsBadAlloc) {
  struct A {
    alignas(0x100000) uint32_t x;
    explicit A(uint32_t x) : x(x) {}
  };

  auto start = reinterpret_cast<std::byte *>(0x10000);
  auto end = reinterpret_cast<std::byte *>(0x20000);
  size_t total_size = end - start;

  RawBumpAllocator raw_alloc{start, total_size};
  BumpAllocator<A> alloc{&raw_alloc};

  ASSERT_THROW(alloc.allocate(1), std::bad_alloc);
  ASSERT_EQ(0, raw_alloc.allocated_bytes());
  ASSERT_EQ(0, alloc.max_size());
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorPtrArithmetic, DeallocateIsANoOp) {
  auto start = reinterpret_cast<std::byte *>(0x10000);
  auto end = reinterpret_cast<std::byte *>(0x20000);
  size_t total_size = end - start;

  RawBumpAllocator raw_alloc{start, total_size};
  BumpAllocator<uint64_t> alloc{&raw_alloc};

  uint64_t *ptr1 = alloc.allocate(16);
  uint64_t *ptr2 = alloc.allocate(2);
  uint64_t *ptr3 = alloc.allocate(100);

  ASSERT_EQ(0x1FF80, reinterpret_cast<size_t>(ptr1));
  ASSERT_EQ(0x1FF70, reinterpret_cast<size_t>(ptr2));
  ASSERT_EQ(0x1FC50, reinterpret_cast<size_t>(ptr3));

  alloc.deallocate(ptr2, 2);
  alloc.deallocate(ptr3, 100);
  alloc.deallocate(ptr1, 16);

  ASSERT_EQ(944, raw_alloc.allocated_bytes());

  uint64_t *ptr4 = alloc.allocate(1);

  ASSERT_EQ(0x1FC48, reinterpret_cast<size_t>(ptr4));
  ASSERT_EQ(952, raw_alloc.allocated_bytes());
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorRealUsage, ViaAllocatorTraitsAPI) {
  
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorRealUsage, WithOneStdVectorReserve) {
  std::byte backing_storage[1024];
  RawBumpAllocator raw_alloc{backing_storage, 1024};
  BumpAllocator<uint64_t> alloc{&raw_alloc};

  size_t max_size = alloc.max_size();
  ASSERT_TRUE(max_size >= 127);

  std::vector<uint64_t, BumpAllocator<uint64_t>> vec{alloc};
  vec.reserve(max_size);

  for (size_t i = 0; i < alloc.max_size(); i++) {
    vec.push_back(i);
  }

  ASSERT_EQ(max_size, vec.size());
}
//---------------------------------------------------------------------------
TEST(BumpAllocatorRealUsage, WithOneStdVectorReallocating) {
  std::byte backing_storage[1024];
  RawBumpAllocator raw_alloc{backing_storage, 1024};
  BumpAllocator<uint64_t> alloc{&raw_alloc};

  size_t max_size = alloc.max_size();
  ASSERT_TRUE(max_size >= 127);

  std::vector<uint64_t, BumpAllocator<uint64_t>> vec{alloc};

  for (size_t i = 0; i < 64; i++) {
    vec.push_back(i);
  }

  ASSERT_EQ(64, vec.size());

  for (size_t i = 0; i < 64; i++) {
    ASSERT_EQ(i, vec[i]);
  }
  ASSERT_TRUE(static_cast<void *>(backing_storage) <= static_cast<void *>(vec.data()));
  ASSERT_TRUE(static_cast<void *>(vec.data() + 64) < static_cast<void *>(backing_storage + 1024));
}
//---------------------------------------------------------------------------
// This is just a fuzz test - run for a while and allocate a bunch of memory, make sure nothing fails.
//---------------------------------------------------------------------------
TEST(BumpAllocatorRealUsage, WithMultipleStdVectorsFuzz) {
  std::vector<std::byte> backing_storage{};
  backing_storage.reserve(1 << 20);
  RawBumpAllocator raw_alloc{backing_storage.data(), 1 << 20};
  BumpAllocator<uint8_t> alloc1{&raw_alloc};
  BumpAllocator<uint16_t> alloc2{&raw_alloc};
  BumpAllocator<uint32_t> alloc4{&raw_alloc};
  BumpAllocator<uint64_t> alloc8{&raw_alloc};
  {
    std::vector<uint8_t, BumpAllocator<uint8_t>> vec1{alloc1};
    std::vector<uint16_t, BumpAllocator<uint16_t>> vec2{alloc2};
    std::vector<uint32_t, BumpAllocator<uint32_t>> vec4{alloc4};
    std::vector<uint64_t, BumpAllocator<uint64_t>> vec8{alloc8};

    std::mt19937 engine{2137};
    std::uniform_int_distribution<size_t> dist{0, 37};
    auto gen = [&engine, &dist] { return dist(engine); };
    size_t iters = 100;

    try {
      while (iters--) {
        for (size_t i = 0; i < gen(); i++) {
          vec2.push_back(i);
        }
        for (size_t i = 0; i < gen(); i++) {
          vec8.push_back(i);
        }
        for (size_t i = 0; i < gen(); i++) {
          vec1.push_back(i);
        }
        for (size_t i = 0; i < gen(); i++) {
          vec4.push_back(i);
        }
      }
    } catch (std::bad_alloc &) {
      // This is expected, we used up all capacity.
      // Make sure this is the case by asserting some vector would not be able to realloc.
      ASSERT_TRUE(alloc1.remaining_size() < 2 * vec1.capacity() || alloc2.remaining_size() < 2 * vec2.capacity() ||
                  alloc4.remaining_size() < 2 * vec4.capacity() || alloc8.remaining_size() < 2 * vec8.capacity());
      vec1.clear();
      vec2.clear();
      vec4.clear();
      vec8.clear();
      raw_alloc.clear();
    }
  }
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}