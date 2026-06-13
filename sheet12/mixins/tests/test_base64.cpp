#include "../base64.hpp"
#include <gtest/gtest.h>
#include <iterator>

using namespace std;

TEST(TestBase64, Empty) {
  unsigned char *ptr = nullptr;
  std::string out = base64Encode(std::span{ptr, 0});
  EXPECT_EQ("", out);

  std::vector<std::byte> roundtrip{};
  base64Decode(out, std::back_inserter(roundtrip));
  EXPECT_TRUE(roundtrip.empty());
}
//---------------------------------------------------------------------------
TEST(TestBase64, OneChar) {
  std::string input = "x";
  std::string out = base64Encode(std::span{reinterpret_cast<const std::byte *>(input.data()), 1});
  EXPECT_EQ("eA==", out);

  std::string roundtrip{};
  base64Decode(out, std::back_inserter(roundtrip));
  EXPECT_EQ(input, roundtrip);
}
//---------------------------------------------------------------------------
TEST(TestBase64, TwoChar) {
  std::string input = "xy";
  std::string out = base64Encode(std::span{reinterpret_cast<const std::byte *>(input.data()), 2});
  EXPECT_EQ("eHk=", out);

  std::string roundtrip{};
  base64Decode(out, std::back_inserter(roundtrip));
  EXPECT_EQ(input, roundtrip);
}
//---------------------------------------------------------------------------
TEST(TestBase64, ThreeChar) {
  std::string input = "xyz";
  std::string out = base64Encode(std::span{reinterpret_cast<const std::byte *>(input.data()), 3});
  EXPECT_EQ("eHl6", out);

  std::string roundtrip{};
  base64Decode(out, std::back_inserter(roundtrip));
  EXPECT_EQ(input, roundtrip);
}
//---------------------------------------------------------------------------
TEST(TestBase64, Longer01) {
  std::string input = "Concepts of C++ Programming";
  std::string out = base64Encode(std::span{reinterpret_cast<const std::byte *>(input.data()), input.size()});
  EXPECT_EQ("Q29uY2VwdHMgb2YgQysrIFByb2dyYW1taW5n", out);

  std::string roundtrip{};
  base64Decode(out, std::back_inserter(roundtrip));
  EXPECT_EQ(input, roundtrip);
}
//---------------------------------------------------------------------------
TEST(TestBase64, Longer02) {
  std::string input = "Rust > C++";
  std::string out = base64Encode(std::span{reinterpret_cast<const std::byte *>(input.data()), input.size()});
  EXPECT_EQ("UnVzdCA+IEMrKw==", out);

  std::string roundtrip{};
  base64Decode(out, std::back_inserter(roundtrip));
  EXPECT_EQ(input, roundtrip);
}