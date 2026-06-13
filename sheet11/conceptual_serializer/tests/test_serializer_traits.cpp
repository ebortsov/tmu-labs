#include "../serializer_traits.hpp"
#include <forward_list>
#include <gtest/gtest.h>
#include <list>
#include <queue>
#include <set>
#include <unordered_set>
#include <vector>

using namespace conceptual;

TEST(ConceptualSerializer, PrimitiveTypesSerializability) {
  static_assert(StringLike<std::string>);
  static_assert(StringLike<const char *>);
  static_assert(StringLike<char>);
  static_assert(Primitive<std::string>);
  static_assert(Primitive<const char *>);
  static_assert(Primitive<char>);
  static_assert(Primitive<double>);
  static_assert(Primitive<float>);
  static_assert(Primitive<uint64_t>);
  static_assert(Primitive<int64_t>);
  static_assert(Primitive<uint32_t>);
  static_assert(Primitive<int32_t>);
  static_assert(Primitive<uint16_t>);
  static_assert(Primitive<int16_t>);
  static_assert(Primitive<uint8_t>);
  static_assert(Primitive<int8_t>);
  static_assert(Primitive<bool>);

  static_assert(Serializable<std::string>);
  static_assert(Serializable<const char *>);
  static_assert(Serializable<char>);
  static_assert(Serializable<double>);
  static_assert(Serializable<float>);
  static_assert(Serializable<uint64_t>);
  static_assert(Serializable<int64_t>);
  static_assert(Serializable<uint32_t>);
  static_assert(Serializable<int32_t>);
  static_assert(Serializable<uint16_t>);
  static_assert(Serializable<int16_t>);
  static_assert(Serializable<uint8_t>);
  static_assert(Serializable<int8_t>);
  static_assert(Serializable<bool>);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, ArrayLikeStdTypesSerializability) {
  static_assert(ArrayLike<std::deque<uint32_t>>);
  static_assert(ArrayLike<std::vector<uint32_t>>);
  static_assert(ArrayLike<std::array<uint32_t, 64>>);

  static_assert(ListSerializable<std::deque<uint32_t>>);
  static_assert(ListSerializable<std::vector<uint32_t>>);
  static_assert(ListSerializable<std::array<uint32_t, 64>>);

  static_assert(Serializable<std::deque<uint32_t>>);
  static_assert(Serializable<std::vector<uint32_t>>);
  static_assert(Serializable<std::array<uint32_t, 64>>);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, SeqLikeStdTypesSerializability) {
  static_assert(SeqLike<std::forward_list<uint32_t>>);
  static_assert(SeqLike<std::list<uint32_t>>);
  static_assert(SeqLike<std::multiset<uint32_t>>);
  static_assert(SeqLike<std::set<uint32_t>>);
  static_assert(SeqLike<std::unordered_set<uint32_t>>);
  static_assert(SeqLike<std::unordered_multiset<uint32_t>>);

  static_assert(ListSerializable<std::forward_list<uint32_t>>);
  static_assert(ListSerializable<std::list<uint32_t>>);
  static_assert(ListSerializable<std::multiset<uint32_t>>);
  static_assert(ListSerializable<std::set<uint32_t>>);
  static_assert(ListSerializable<std::unordered_set<uint32_t>>);
  static_assert(ListSerializable<std::unordered_multiset<uint32_t>>);

  static_assert(Serializable<std::forward_list<uint32_t>>);
  static_assert(Serializable<std::list<uint32_t>>);
  static_assert(Serializable<std::multiset<uint32_t>>);
  static_assert(Serializable<std::set<uint32_t>>);
  static_assert(Serializable<std::unordered_set<uint32_t>>);
  static_assert(Serializable<std::unordered_multiset<uint32_t>>);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, MapLikeStdTypesSerializability) {
  static_assert(MapLike<std::map<std::string, uint32_t>>);
  static_assert(MapLike<std::map<uint32_t, uint32_t>>);
  static_assert(MapLike<std::unordered_map<std::string, uint32_t>>);
  static_assert(MapLike<std::unordered_map<uint32_t, uint32_t>>);

  static_assert(Serializable<std::map<std::string, uint32_t>>);
  static_assert(Serializable<std::map<uint32_t, uint32_t>>);
  static_assert(Serializable<std::unordered_map<std::string, uint32_t>>);
  static_assert(Serializable<std::unordered_map<uint32_t, uint32_t>>);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, NestedStdTypeSerializability) {
  static_assert(Serializable<std::map<std::string,
          std::vector<std::unordered_map<std::string, std::array<std::unordered_map<char, std::string>, 64>>>>>);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, OptionalPrimitiveSerializability) {
  static_assert(Serializable<std::optional<std::string>>);
  static_assert(Serializable<std::optional<char>>);
  static_assert(Serializable<std::optional<double>>);
  static_assert(Serializable<std::optional<float>>);
  static_assert(Serializable<std::optional<uint64_t>>);
  static_assert(Serializable<std::optional<int64_t>>);
  static_assert(Serializable<std::optional<uint32_t>>);
  static_assert(Serializable<std::optional<int32_t>>);
  static_assert(Serializable<std::optional<uint16_t>>);
  static_assert(Serializable<std::optional<int16_t>>);
  static_assert(Serializable<std::optional<uint8_t>>);
  static_assert(Serializable<std::optional<int8_t>>);
  static_assert(Serializable<std::optional<bool>>);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, OptionalContainerSerializability) {
  static_assert(Serializable<std::optional<std::deque<uint32_t>>>);
  static_assert(Serializable<std::optional<std::vector<uint32_t>>>);
  static_assert(Serializable<std::optional<std::array<uint32_t, 64>>>);
  static_assert(Serializable<std::optional<std::forward_list<uint32_t>>>);
  static_assert(Serializable<std::optional<std::list<uint32_t>>>);
  static_assert(Serializable<std::optional<std::multiset<uint32_t>>>);
  static_assert(Serializable<std::optional<std::set<uint32_t>>>);
  static_assert(Serializable<std::optional<std::unordered_set<uint32_t>>>);
  static_assert(Serializable<std::optional<std::unordered_multiset<uint32_t>>>);
  static_assert(Serializable<std::optional<std::map<std::string, uint32_t>>>);
  static_assert(Serializable<std::optional<std::map<uint32_t, uint32_t>>>);
  static_assert(Serializable<std::optional<std::unordered_map<std::string, uint32_t>>>);
  static_assert(Serializable<std::optional<std::unordered_map<uint32_t, uint32_t>>>);
}
//---------------------------------------------------------------------------
TEST(ConceptualSerializer, ContainerOfOptionalsSerializability) {
  static_assert(Serializable<std::deque<std::optional<uint32_t>>>);
  static_assert(Serializable<std::vector<std::optional<uint32_t>>>);
  static_assert(Serializable<std::array<std::optional<uint32_t>, 64>>);
  static_assert(Serializable<std::forward_list<std::optional<uint32_t>>>);
  static_assert(Serializable<std::list<std::optional<uint32_t>>>);
  static_assert(Serializable<std::multiset<std::optional<uint32_t>>>);
  static_assert(Serializable<std::set<std::optional<uint32_t>>>);
  static_assert(Serializable<std::unordered_set<std::optional<uint32_t>>>);
  static_assert(Serializable<std::unordered_multiset<std::optional<uint32_t>>>);
  static_assert(Serializable<std::map<std::string, std::optional<uint32_t>>>);
  static_assert(Serializable<std::map<uint32_t, std::optional<uint32_t>>>);
  static_assert(Serializable<std::unordered_map<std::string, std::optional<uint32_t>>>);
  static_assert(Serializable<std::unordered_map<uint32_t, std::optional<uint32_t>>>);
}