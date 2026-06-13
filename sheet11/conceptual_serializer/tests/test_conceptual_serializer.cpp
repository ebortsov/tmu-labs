#include "../serializer.hpp"
#include <forward_list>
#include <gtest/gtest.h>
#include <list>
#include <set>
#include <unordered_set>
#include <vector>

using namespace conceptual;

template <typename I, typename U>
void assert_eq_one_of(I expected, U actual) {
  bool any_eq = false;
  for (const auto &ex : expected) {
    if (ex == actual) {
      any_eq = true;
      break;
    }
  }
  auto format_out = [&] {
    std::string msg = "expected one of: ";
    for (const auto &ex : expected) {
      msg += std::format("{} ", ex);
    }
    msg += std::format("; found {}", actual);
    return msg;
  };

  ASSERT_TRUE(any_eq) << format_out();
}
//---------------------------------------------------------------------------
template <typename T, typename U>
void assert_eq_one_of(std::initializer_list<T> expected, U actual) {
  assert_eq_one_of<std::initializer_list<T>, U>(expected, actual);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, ReadmeTest01) {
  ASSERT_EQ("42", conceptual::serialize(42));
  ASSERT_EQ("true", conceptual::serialize(true));
  ASSERT_EQ("42.000000", conceptual::serialize(42.0));
  ASSERT_EQ("\"string value\"", conceptual::serialize("string value"));
  ASSERT_EQ("\"a\"", conceptual::serialize('a'));
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, ReadmeTest02) {
  ASSERT_EQ("[true]", conceptual::serialize(std::vector<bool>{true}));
  ASSERT_EQ("[\"a\",\"b\",\"c\"]", conceptual::serialize(std::list<char>{'a', 'b', 'c'}));
  ASSERT_EQ("[]", conceptual::serialize(std::set<uint32_t>{}));
  ASSERT_EQ("[[1,2],[],[1]]", conceptual::serialize(std::vector<std::vector<uint32_t>>{{1, 2}, {}, {1}}));
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, ReadmeTest03) {
  ASSERT_EQ("{}", conceptual::serialize(std::map<bool, bool>{}));
  ASSERT_EQ("{\"a\":42}", conceptual::serialize(std::map<char, uint32_t>{{'a', 42}}));

  assert_eq_one_of({R"({"0":1,"1":2})", R"({"0":1,"1":3})", R"({"1":2,"0":1})", R"({"1":3,"0":1})"},
      conceptual::serialize(std::multimap<uint32_t, uint32_t>{
          {0, 1},
          {1, 2},
          {1, 3},
      }));
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, ReadmeTest04) {
  ASSERT_EQ("[true]", conceptual::serialize(std::vector<bool>{true}));
  ASSERT_EQ("[\"a\",\"b\",\"c\"]", conceptual::serialize(std::list<char>{'a', 'b', 'c'}));
  ASSERT_EQ("[]", conceptual::serialize(std::set<uint32_t>{}));
  ASSERT_EQ("[[1,2],[],[1]]", conceptual::serialize(std::vector<std::vector<uint32_t>>{{1, 2}, {}, {1}}));
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, ReadmeTest05) {
  ASSERT_EQ("42", conceptual::serialize(std::optional<uint32_t>{42}));
  ASSERT_EQ("null", conceptual::serialize(std::optional<uint32_t>{}));
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeCString) {
  std::string value = serialize("bingo");

  ASSERT_EQ("\"bingo\"", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeString) {
  std::string input = "bingo";
  std::string value = serialize(input);

  ASSERT_EQ("\"bingo\"", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeInteger) {
  uint64_t input = 42;
  std::string value = serialize(input);

  ASSERT_EQ("42", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeFloat) {
  float input = 42.0;
  std::string value = serialize(input);

  ASSERT_EQ("42.000000", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeDouble) {
  double input = 42.0;
  std::string value = serialize(input);

  ASSERT_EQ("42.000000", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeLargeDouble) {
  double input = 42.5e18;
  std::string value = serialize(input);

  ASSERT_EQ("42500000000000000000.000000", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeChar) {
  char input = 'a';
  std::string value = serialize(input);

  ASSERT_EQ("\"a\"", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleArray) {
  std::array<uint32_t, 5> input = {1, 3, 7, 42, 0};
  std::string value = serialize(input);

  ASSERT_EQ("[1,3,7,42,0]", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleVector) {
  std::vector<uint64_t> input = {1, 3, 7, 42, 0};
  std::string value = serialize(input);

  ASSERT_EQ("[1,3,7,42,0]", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeNestedArrayLikes) {
  std::vector<std::array<bool, 3>> inner1 = {
      {false, false, false}, {false, false, true}, {false, true, false}, {false, true, true}};
  std::vector<std::array<bool, 3>> inner2 = {{true, false, false}};
  std::vector<std::array<bool, 3>> inner3 = {{true, false, true}};
  std::vector<std::array<bool, 3>> inner4 = {};
  std::vector<std::array<bool, 3>> inner5 = {{true, true, false}, {true, true, true}};

  std::vector input = {inner1, inner2, inner3, inner4, inner5};
  std::string value = serialize(input);

  ASSERT_EQ("[[[false,false,false],[false,false,true],[false,true,false],[false,true,true]],[[true,false,false]],[["
            "true,false,true]],[],[[true,true,false],[true,true,true]]]",
      value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleForwardList) {
  std::forward_list<int32_t> input = {1, 3, -7, -42, 0};
  std::string value = serialize(input);

  ASSERT_EQ("[1,3,-7,-42,0]", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleList) {
  std::forward_list<int16_t> input = {1, 3, -7, -42, 0};
  std::string value = serialize(input);

  ASSERT_EQ("[1,3,-7,-42,0]", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleSet) {
  std::set<uint8_t> input = {1, 4, 2, 1, 5, 3, 1, 2};
  std::string value = serialize(input);

  ASSERT_EQ("[1,2,3,4,5]", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleMultiset) {
  std::multiset<uint8_t> input = {1, 4, 2, 1, 5, 3, 1, 2};
  std::string value = serialize(input);

  ASSERT_EQ("[1,1,1,2,2,3,4,5]", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleUnorderedSet) {
  std::unordered_set<char> input = {'a', 'b', 'c'};
  std::string value = serialize(input);

  assert_eq_one_of({R"(["a","b","c"])", R"(["a","c","b"])", R"(["b","a","c"])", R"(["b","c","a"])", R"(["c","a","b"])",
                       R"(["c","b","a"])"},
      value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleUnorderedMultiset) {
  std::unordered_multiset<char> input = {'a', 'b', 'a'};
  std::string value = serialize(input);

  assert_eq_one_of({R"(["a","a","b"])", R"(["a","b","a"])", R"(["b","a","a"])"}, value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleMap) {
  std::map<std::string, std::string> input = {{"key1", "value1"}, {"key2", "value2"}, {"key3", "value3"}};
  std::string value = serialize(input);

  assert_eq_one_of(
      {R"({"key1":"value1","key2":"value2","key3":"value3"})", R"({"key1":"value1","key3":"value3","key2":"value2"})",
          R"({"key2":"value2","key1":"value1","key3":"value3"})",
          R"({"key2":"value2","key3":"value3","key1":"value1"})",
          R"({"key3":"value3","key1":"value1","key2":"value2"})",
          R"({"key3":"value3","key2":"value2","key1":"value1"})"},
      value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleMultimap) {
  std::multimap<std::string, std::string> input = {{"key1", "value1"}, {"key1", "value2"}, {"key2", "value3"}};
  std::string value = serialize(input);

  assert_eq_one_of({R"({"key1":"value1","key2":"value3"})", R"({"key2":"value3","key1":"value1"]})",
                       R"({"key1":"value2","key2":"value3"})", R"({"key2":"value3","key1":"value2"})"},
      value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleUnorderedMap) {
  std::unordered_map<std::string, std::string> input = {{"key1", "value1"}, {"key2", "value2"}, {"key3", "value3"}};
  std::string value = serialize(input);

  assert_eq_one_of(
      {R"({"key1":"value1","key2":"value2","key3":"value3"})", R"({"key1":"value1","key3":"value3","key2":"value2"})",
          R"({"key2":"value2","key1":"value1","key3":"value3"})",
          R"({"key2":"value2","key3":"value3","key1":"value1"})",
          R"({"key3":"value3","key1":"value1","key2":"value2"})",
          R"({"key3":"value3","key2":"value2","key1":"value1"})"},
      value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeSimpleUnorderedMultimap) {
  std::unordered_multimap<std::string, std::string> input = {
      {"key1", "value1"}, {"key1", "value2"}, {"key2", "value3"}};
  std::string value = serialize(input);

  assert_eq_one_of({R"({"key1":"value1","key2":"value3"})", R"({"key2":"value3","key1":"value1"})",
                       R"({"key1":"value2","key2":"value3"})", R"({"key2":"value3","key1":"value2"})"},
      value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeNonNullOptionalPrimitive) {
  std::optional<uint32_t> input = 42;
  std::string value = serialize(input);

  ASSERT_EQ("42", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeNullOptionalPrimitive) {
  std::optional<uint32_t> input = {};
  std::string value = serialize(input);

  ASSERT_EQ("null", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeNonNullOptionalVector) {
  std::optional<std::vector<uint32_t>> input = {{}};
  std::string value = serialize(input);

  ASSERT_EQ("[]", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeNullOptionalVector) {
  std::optional<std::vector<uint32_t>> input = {};
  std::string value = serialize(input);

  ASSERT_EQ("null", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SerializeVectorWithOptionalValues) {
  std::vector<std::optional<uint32_t>> input = {7, 0, {}, 4, 3, {}, 22};
  std::string value = serialize(input);

  ASSERT_EQ("[7,0,null,4,3,null,22]", value);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, MixedTest01) {
  std::map<std::string, std::map<std::string, std::optional<std::vector<uint64_t>>>> input = {
      {"group1", {{"user1", {}}, {"user2", {{}}}, {"user3", {{1, 7, 99193}}}}}, {"group2", {{"user4", {{1, 2}}}}},
      {"group3", {}}};
  static_assert(Serializable<decltype(input)>);
  std::string value = serialize(input);
  std::vector<std::string> expected = {};
  std::vector<std::string> expected_inner = {
      R"({"user1":null,"user2":[],"user3":[1,7,99193]})",
      R"({"user1":null,"user3":[1,7,99193],"user2":[]})",
      R"({"user2":[],"user1":null,"user3":[1,7,99193]})",
      R"({"user2":[],"user3":[1,7,99193],"user1":null})",
      R"({"user3":[1,7,99193],"user1":null,"user2":[]})",
      R"({"user3":[1,7,99193],"user2":[],"user1":null})",
  };
  for (auto inner : expected_inner) {
    expected.emplace_back(std::format(R"({{"group1":{},"group2":{{"user4":[1,2]}},"group3":{{}}}})", inner));
    expected.emplace_back(std::format(R"({{"group1":{},"group3":{{}},"group2":{{"user4":[1,2]}}}})", inner));
    expected.emplace_back(std::format(R"({{"group2":{{"user4":[1,2]}},"group1":{},"group3":{{}}}})", inner));
    expected.emplace_back(std::format(R"({{"group2":{{"user4":[1,2]}},"group3":{{}},"group1":{}}})", inner));
    expected.emplace_back(std::format(R"({{"group3":{{}},"group1":{},"group2":{{"user4":[1,2]}}}})", inner));
    expected.emplace_back(std::format(R"({{"group3":{{}},"group2":{{"user4":[1,2]}},"group1":{}}})", inner));
  }
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, MixedTest02) {
  std::vector<std::map<uint32_t, std::vector<std::optional<float>>>> input = {
      {{42, {3.142, 2.718, {}, 0.007}}, {17, {}}, {0, {0.42}}}};
  static_assert(Serializable<decltype(input)>);
  std::string value = serialize(input);

  assert_eq_one_of({R"([{"0":[0.420000],"17":[],"42":[3.142000,2.718000,null,0.007000]}])",
                       R"([{"0":[0.420000],"42":[3.142000,2.718000,null,0.007000],"17":[]}])",
                       R"([{"17":[],"0":[0.420000],"42":[3.142000,2.718000,null,0.007000]}])",
                       R"([{"17":[],"42":[3.142000,2.718000,null,0.007000],"0":[0.420000]}])",
                       R"([{"42":[3.142000,2.718000,null,0.007000],"0":[0.420000],"17":[]}])",
                       R"([{"42":[3.142000,2.718000,null,0.007000],"17":[],"0":[0.420000]}])"},
      value);
}
//---------------------------------------------------------------------------
namespace {
class CustomSeqLikeIter {
public:
  using difference_type = ptrdiff_t;
  using value_type = const char *;
  static CustomSeqLikeIter sentinel() {
    auto iter = CustomSeqLikeIter{};
    iter.idx = 4;
    return iter;
  }
  CustomSeqLikeIter &operator++() {
    ++idx;
    return *this;
  }
  CustomSeqLikeIter operator++(int) {
    auto ret = *this;
    ++idx;
    return ret;
  }
  const char *operator*() const {
    switch (idx) {
      case 0:
        return "my";
      case 1:
        return "custom";
      case 2:
        return "seq";
      case 3:
        return "impl";
      default:
        return "";
    }
  }
  bool operator==(const CustomSeqLikeIter &iter) const { return idx == iter.idx; }
  bool operator!=(const CustomSeqLikeIter &iter) const { return idx != iter.idx; }

private:
  size_t idx = 0;
};
class CustomSeqLike {
public:
  CustomSeqLikeIter begin() const { return CustomSeqLikeIter{}; }
  CustomSeqLikeIter end() const { return CustomSeqLikeIter::sentinel(); }
};
} // namespace
TEST(ConceptualSerializer, CustomSeqLike) {
  CustomSeqLike input{};
  static_assert(Serializable<decltype(input)>);
  static_assert(SeqLike<decltype(input)>);
  static_assert(ListSerializable<decltype(input)>);
  std::string value = serialize(input);

  ASSERT_EQ(R"(["my","custom","seq","impl"])", value);
}
//---------------------------------------------------------------------------
namespace {
class CustomArrayLike {
public:
  CustomSeqLikeIter begin() const { throw std::runtime_error("should not be called"); }
  CustomSeqLikeIter end() const { throw std::runtime_error("should not be called"); }

  size_t size() const { return 4; }

  const char *operator[](size_t idx) const {
    switch (idx) {
      case 0:
        return "my";
      case 1:
        return "custom";
      case 2:
        return "array";
      case 3:
        return "impl";
      default:
        return "";
    }
  }
};
} // namespace
TEST(ConceptualSerializer, CustomArrayLike) {
  CustomArrayLike input{};
  static_assert(Serializable<decltype(input)>);
  static_assert(ArrayLike<decltype(input)>);
  static_assert(ListSerializable<decltype(input)>);
  std::string value = serialize(input);

  ASSERT_EQ(R"(["my","custom","array","impl"])", value);
}
//---------------------------------------------------------------------------
namespace {
struct CustomKeyValuePair {
  const char *first;
  const char *second;
  CustomKeyValuePair *operator->() { return this; }
};
class CustomMapLikeIter {
public:
  using difference_type = ptrdiff_t;
  using value_type = CustomKeyValuePair;
  static CustomMapLikeIter sentinel() {
    auto iter = CustomMapLikeIter{};
    iter.idx = 2;
    return iter;
  }
  CustomMapLikeIter &operator++() {
    ++idx;
    return *this;
  }
  CustomMapLikeIter operator++(int) {
    auto ret = *this;
    ++idx;
    return ret;
  }
  CustomKeyValuePair operator*() const {
    switch (idx) {
      case 0:
        return CustomKeyValuePair{.first = "my", .second = "map"};
      case 1:
        return CustomKeyValuePair{.first = "is", .second = "awesome"};
      default:
        return CustomKeyValuePair{};
    }
  }
  CustomKeyValuePair operator->() const { return this->operator*(); }
  bool operator==(const CustomMapLikeIter &iter) const { return idx == iter.idx; }
  bool operator!=(const CustomMapLikeIter &iter) const { return idx != iter.idx; }

private:
  size_t idx = 0;
};
class CustomMapLike {
public:
  using key_type = const char *;
  using mapped_type = const char *;

  CustomMapLikeIter begin() const { return CustomMapLikeIter{}; }
  CustomMapLikeIter end() const { return CustomMapLikeIter::sentinel(); }
};
} // namespace
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, CustomSimpleMapLike) {
  CustomMapLike input{};
  static_assert(Serializable<decltype(input)>);
  static_assert(MapLike<decltype(input)>);
  std::string value = serialize(input);

  assert_eq_one_of({R"({"my":"map","is":"awesome"})", R"({"is":"awesome","my":"map"})"}, value);
}
//---------------------------------------------------------------------------
namespace {
class CustomMultimapLikeIter {
public:
  using difference_type = ptrdiff_t;
  using value_type = CustomKeyValuePair;
  static CustomMultimapLikeIter sentinel() {
    auto iter = CustomMultimapLikeIter{};
    iter.idx = 3;
    return iter;
  }
  CustomMultimapLikeIter &operator++() {
    ++idx;
    return *this;
  }
  CustomMultimapLikeIter operator++(int) {
    auto ret = *this;
    ++idx;
    return ret;
  }
  CustomKeyValuePair operator*() const {
    switch (idx) {
      case 0:
        return CustomKeyValuePair{.first = "my", .second = "map"};
      case 1:
        return CustomKeyValuePair{.first = "is", .second = "awesome"};
      case 2:
        return CustomKeyValuePair{.first = "is", .second = "multivalued"};
      default:
        return CustomKeyValuePair{};
    }
  }
  CustomKeyValuePair operator->() const { return this->operator*(); }
  bool operator==(const CustomMultimapLikeIter &iter) const { return idx == iter.idx; }
  bool operator!=(const CustomMultimapLikeIter &iter) const { return idx != iter.idx; }

private:
  size_t idx = 0;
};
class CustomMultimapLike {
public:
  using key_type = const char *;
  using mapped_type = const char *;

  CustomMultimapLikeIter begin() const { return CustomMultimapLikeIter{}; }
  CustomMultimapLikeIter end() const { return CustomMultimapLikeIter::sentinel(); }
};
} // namespace
TEST(ConceptualSerializer, CustomMultimapLike) {
  CustomMultimapLike input{};
  static_assert(Serializable<decltype(input)>);
  static_assert(MapLike<decltype(input)>);
  std::string value = serialize(input);

  assert_eq_one_of({R"({"my":"map","is":"awesome"})", R"({"my":"map","is":"multivalued"})",
                       R"({"is":"awesome","my":"map"})", R"({"is":"multivalued","my":"map"})"},
      value);
}