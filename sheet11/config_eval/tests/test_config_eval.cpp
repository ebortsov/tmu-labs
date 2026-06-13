#include "../config_concept.hpp"
#include "../config_eval.hpp"
#include <gtest/gtest.h>

using namespace config_eval;
using namespace std::literals;

TEST(ConfigEval, Empty) {
  static constexpr char str[] = "";
  constexpr auto structure = parse_config<str>();
  static_assert(Config<decltype(structure)>);

  ASSERT_EQ(0, structure.size());

  constexpr auto value = structure.get("any_key");
  ASSERT_EQ(value, "");
}
#define EXPECT_THROW_WITH_MESSAGE(statement, txt)                                                                      \
  try {                                                                                                                \
    statement;                                                                                                         \
    FAIL() << "Expected exception!";                                                                                   \
  } catch (std::out_of_range & msg) {                                                                                  \
    ASSERT_EQ(std::string_view(msg.what()), txt);                                                                      \
  } catch (...) {                                                                                                      \
    FAIL() << "Expected runtime error!";                                                                               \
  }
//---------------------------------------------------------------------------
TEST(ConfigEval, RuntimeTest) {
  static constexpr char str[] = "key1=value1 key2=value2 key3=value3";
  constexpr auto structure = parse_config<str>();
  static_assert(Config<decltype(structure)>);

  ASSERT_EQ(3, structure.size());
  ASSERT_TRUE(structure.has_key("key1"));
  ASSERT_TRUE(structure.has_key("key2"));
  ASSERT_TRUE(structure.has_key("key3"));
  ASSERT_FALSE(structure.has_key("key4"));
  ASSERT_EQ("value1", structure["key1"]);
  ASSERT_EQ("value2", structure["key2"]);
  ASSERT_EQ("value3", structure["key3"]);
  ASSERT_EQ("value1", structure.get("key1"));
  ASSERT_EQ("value2", structure.get("key2"));
  ASSERT_EQ("value3", structure.get("key3"));
  EXPECT_THROW_WITH_MESSAGE(structure.get("key4"), "key not present");
  constexpr auto missing_value = structure.get("key4");
  ASSERT_EQ(missing_value, "");
}
//---------------------------------------------------------------------------
template <typename T>
consteval void verify_kv(const T &config, std::string_view k, std::string_view v)
  requires Config<T>
{
  constexpr bool has_key = config.has_key(k);
  constexpr std::string_view value_sq = config[k];
  constexpr std::string_view value_get = config.get(k);

  ASSERT_TRUE(has_key);
  ASSERT_EQ(v, value_sq);
  ASSERT_EQ(v, value_get);
}
//---------------------------------------------------------------------------
TEST(ConfigEval, ConstexprTest) {
  static constexpr char str[] = "level=debug dir=/var/log/cppprog timeout=60 format=json";
  constexpr auto structure = parse_config<str>();
  static_assert(Config<decltype(structure)>);

  constexpr size_t size = structure.size();
  ASSERT_EQ(4, size);
#define VERIFY_KV(k, v)                                                                                                \
  do {                                                                                                                 \
    constexpr bool has_key = structure.has_key(k);                                                                     \
    constexpr std::string_view value_sq = structure[k];                                                                \
    constexpr std::string_view value_get = structure.get(k);                                                           \
    ASSERT_TRUE(has_key);                                                                                              \
    ASSERT_EQ(v, value_sq);                                                                                            \
    ASSERT_EQ(v, value_get);                                                                                           \
  } while (0);

  VERIFY_KV("level"sv, "debug"sv);
  VERIFY_KV("dir"sv, "/var/log/cppprog"sv);
  VERIFY_KV("timeout"sv, "60"sv);
  VERIFY_KV("format"sv, "json"sv);
#undef VERIFY_KV
}
//---------------------------------------------------------------------------
TEST(ConfigEval, ConstexprTestWithNewline) {
  static constexpr char str[] = "level=debug\n"
                                "dir=/var/log/cppprog\n"
                                "timeout=60\n"
                                "format=json";
  constexpr auto structure = parse_config<str>();
  static_assert(Config<decltype(structure)>);

  constexpr size_t size = structure.size();
  ASSERT_EQ(4, size);
#define VERIFY_KV(k, v)                                                                                                \
  do {                                                                                                                 \
    constexpr bool has_key = structure.has_key(k);                                                                     \
    constexpr std::string_view value_sq = structure[k];                                                                \
    constexpr std::string_view value_get = structure.get(k);                                                           \
    ASSERT_TRUE(has_key);                                                                                              \
    ASSERT_EQ(v, value_sq);                                                                                            \
    ASSERT_EQ(v, value_get);                                                                                           \
  } while (0);

  VERIFY_KV("level"sv, "debug"sv);
  VERIFY_KV("dir"sv, "/var/log/cppprog"sv);
  VERIFY_KV("timeout"sv, "60"sv);
  VERIFY_KV("format"sv, "json"sv);
#undef VERIFY_KV
}
//---------------------------------------------------------------------------
TEST(ConfigEval, MixedNewlines) {
  static constexpr char str[] = "    level=debug \n \n "
                                "dir=/var/log/cppprog\n   ";
  constexpr auto structure = parse_config<str>();
  static_assert(Config<decltype(structure)>);

  constexpr size_t size = structure.size();
  ASSERT_EQ(2, size);
#define VERIFY_KV(k, v)                                                                                                \
  do {                                                                                                                 \
    constexpr bool has_key = structure.has_key(k);                                                                     \
    constexpr std::string_view value_sq = structure[k];                                                                \
    constexpr std::string_view value_get = structure.get(k);                                                           \
    ASSERT_TRUE(has_key);                                                                                              \
    ASSERT_EQ(v, value_sq);                                                                                            \
    ASSERT_EQ(v, value_get);                                                                                           \
  } while (0);

  VERIFY_KV("level"sv, "debug"sv);
  VERIFY_KV("dir"sv, "/var/log/cppprog"sv);
#undef VERIFY_KV
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}