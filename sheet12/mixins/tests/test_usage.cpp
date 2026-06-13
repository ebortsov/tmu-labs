#include "../usage.hpp"
#include <gtest/gtest.h>
#include <iterator>

using namespace std;
using namespace usage;

template <typename T>
concept Hashable = requires(const T &t) {
  { std::hash<T>{}(t) } -> std::same_as<size_t>;
};

TEST(TestUsage, TestCustomer) {
  Customer customer1{"Alexis"};
  Customer customer2{"The Tall One"};
  Customer customer3{"The Other One"};

  customer1.addPurchase(0.5);
  customer1.addPurchase(2.33);
  customer1.addPurchase(1.5);
  customer2.addPurchase(0.1);
  customer2.addPurchase(0.15);
  customer3.addPurchase(50.0);

  {
    SCOPED_TRACE("CountInstances");
    EXPECT_EQ(3, Customer::getInstanceCount());
  }
  {
    SCOPED_TRACE("TypeId");
    EXPECT_EQ(customer1.getTypeId(), customer2.getTypeId());
    EXPECT_EQ(customer2.getTypeId(), customer3.getTypeId());
  }
  {
    SCOPED_TRACE("UniqueId");
    EXPECT_EQ("Alexis", customer1.getId());
    EXPECT_EQ("The Tall One", customer2.getId());
    EXPECT_EQ("The Other One", customer3.getId());
    static_assert(std::equality_comparable<Customer>);
    static_assert(Hashable<Customer>);

    EXPECT_EQ(customer1, customer1);
    EXPECT_EQ(customer2, customer2);
    EXPECT_EQ(customer3, customer3);
    EXPECT_NE(customer1, customer2);
    EXPECT_NE(customer1, customer3);
    EXPECT_NE(customer2, customer3);
    {
      SCOPED_TRACE("objects with equal id hash equally");
      std::hash<Customer> hasher{};
      Customer customer4 = Customer{"Alexis"};
      EXPECT_EQ(customer4, customer1);
      EXPECT_EQ(hasher(customer1), hasher(customer4));
    }

    {
      SCOPED_TRACE("hash object is equivalent to hash id");
      std::hash<Customer> customerHasher{};
      std::hash<std::string> idHasher{};
      size_t customerHash1 = customerHasher(customer1);
      size_t idHash1 = idHasher(customer1.getId());
      size_t customerHash2 = customerHasher(customer2);
      size_t idHash2 = idHasher(customer2.getId());
      size_t customerHash3 = customerHasher(customer3);
      size_t idHash3 = idHasher(customer3.getId());

      EXPECT_EQ(idHash1, customerHash1);
      EXPECT_EQ(idHash2, customerHash2);
      EXPECT_EQ(idHash3, customerHash3);
    }
  }
}

TEST(TestUsage, TestSummary) {
  Summary summary1{3, 10, 123.45};
  Summary summary2{2, 2, 13.95};
  Summary summary3{1, 7, 500.99};

  {
    SCOPED_TRACE("CountInstances");
    EXPECT_EQ(3, Summary::getInstanceCount());
  }

  {
    SCOPED_TRACE("TypeId");
    EXPECT_EQ(summary1.getTypeId(), summary2.getTypeId());
    EXPECT_EQ(summary2.getTypeId(), summary3.getTypeId());
  }
  {
    SCOPED_TRACE("BitRepr");
    {
      SCOPED_TRACE("Copy");
      Summary summary4{summary1};
      EXPECT_EQ(summary1, summary4);

      SCOPED_TRACE("Move");
      Summary summary5{std::move(summary4)};
      EXPECT_EQ(summary1, summary5);
    }
    std::vector expectedBytes = {byte(0x03), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0x0A),
        byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0xCD), byte(0xCC), byte(0xCC), byte(0xCC),
        byte(0xCC), byte(0xDC), byte(0x5E), byte(0x40)};
    {
      SCOPED_TRACE("asBytes");
      const Summary summary4{summary1};
      Summary summary5{summary1};

      std::span<const std::byte> span4 = summary4.asBytes();
      std::span<std::byte> span5 = summary5.asBytes();

      std::vector<std::byte> vec4{}, vec5{};
      vec4.assign(span4.begin(), span4.end());
      vec5.assign(span5.begin(), span5.end());

      EXPECT_EQ(vec4, vec5);
      EXPECT_EQ(vec4, expectedBytes);

      span5[8] = byte(0x04);
      span5[9] = byte(0x03);
      span5[10] = byte(0x02);
      span5[11] = byte(0x01);
      EXPECT_EQ(0x01020304, summary5.getTotalPurchases());
    }
    {
      SCOPED_TRACE("readBinary");
      Summary summary4 = Summary::readBinary(summary1.asBytes());
      EXPECT_EQ(summary1, summary4);
    }
    {
      SCOPED_TRACE("writeBinary");
      std::vector<std::byte> vec1{};
      summary1.writeBinary(std::back_inserter(vec1));
      EXPECT_EQ(expectedBytes, vec1);
    }
    {
      SCOPED_TRACE("writeBase64");
      std::string s = summary1.writeBase64();
      EXPECT_EQ("AwAAAAAAAAAKAAAAAAAAAM3MzMzM3F5A", s);
    }
    {
      SCOPED_TRACE("readBase64");
      std::string s = "AwAAAAAAAAAKAAAAAAAAAM3MzMzM3F5A";
      Summary summary4 = Summary::readBase64(s);
      EXPECT_EQ(summary1, summary4);
    }
  }
}

TEST(TestUsage, TestItem) {
  Item item1{1, 1, 0.5};
  Item item2{2, 2, 0.1};
  Item item3{3, 1, 100.0};

  {
    SCOPED_TRACE("CountInstances");
    EXPECT_EQ(3, Item::getInstanceCount());
  }
  {
    SCOPED_TRACE("TypeId");
    EXPECT_EQ(item1.getTypeId(), item2.getTypeId());
    EXPECT_EQ(item2.getTypeId(), item3.getTypeId());
  }
  {
    SCOPED_TRACE("UniqueId");
    EXPECT_EQ(1, item1.getId());
    EXPECT_EQ(2, item2.getId());
    EXPECT_EQ(3, item3.getId());
    static_assert(std::equality_comparable<Item>);
    static_assert(Hashable<Item>);

    EXPECT_EQ(item1, item1);
    EXPECT_EQ(item2, item2);
    EXPECT_EQ(item3, item3);
    EXPECT_NE(item1, item2);
    EXPECT_NE(item1, item3);
    EXPECT_NE(item2, item3);
    {
      SCOPED_TRACE("objects with equal id hash equally");
      std::hash<Item> hasher{};
      Item item4 = Item{1, 1, 0.5};
      EXPECT_EQ(item4, item1);
      EXPECT_EQ(hasher(item1), hasher(item4));
    }

    {
      SCOPED_TRACE("hash object is equivalent to hash id");
      std::hash<Item> itemHasher{};
      std::hash<size_t> idHasher{};
      size_t itemHash1 = itemHasher(item1);
      size_t idHash1 = idHasher(item1.getId());
      size_t itemHash2 = itemHasher(item2);
      size_t idHash2 = idHasher(item2.getId());
      size_t itemHash3 = itemHasher(item3);
      size_t idHash3 = idHasher(item3.getId());

      EXPECT_EQ(idHash1, itemHash1);
      EXPECT_EQ(idHash2, itemHash2);
      EXPECT_EQ(idHash3, itemHash3);
    }
  }

  {
    SCOPED_TRACE("BitRepr");
    {
      SCOPED_TRACE("Copy");
      Item item4{item1};
      EXPECT_EQ(item1, item4);

      SCOPED_TRACE("Move");
      Item item5{std::move(item4)};
      EXPECT_EQ(item1, item5);
    }
    std::vector expectedSuffix = {byte(0x01), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0x01),
        byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0), byte(0),
        byte(0), byte(0xE0), byte(0x3F)};
    {
      SCOPED_TRACE("asBytes");
      const Item item4{item1};
      Item item5{item1};

      std::span<const std::byte> span4 = item4.asBytes();
      std::span<std::byte> span5 = item5.asBytes();

      std::vector<std::byte> vec4{}, vec5{}, suffix{};
      vec4.assign(span4.begin(), span4.end());
      vec5.assign(span5.begin(), span5.end());

      // Skip the 8-byte header for virtual dispatch that we don't control.
      suffix.assign(span4.begin() + 8, span4.end());

      EXPECT_EQ(vec4, vec5);
      EXPECT_EQ(suffix, expectedSuffix);

      span5[16] = byte(0x04);
      span5[17] = byte(0x03);
      span5[18] = byte(0x02);
      span5[19] = byte(0x01);
      EXPECT_EQ(0x01020304, item5.getQuantity());
    }
    {
      SCOPED_TRACE("readBinary");
      Item item4 = Item::readBinary(item1.asBytes());
      EXPECT_EQ(item1, item4);
    }
    {
      SCOPED_TRACE("writeBinary");
      std::vector<std::byte> vec1{};
      item1.writeBinary(std::back_inserter(vec1));
      vec1.erase(vec1.begin(), vec1.begin() + 8);
      EXPECT_EQ(expectedSuffix, vec1);
    }
    {
      SCOPED_TRACE("writeBase64 and readBase64");
      std::string s = item1.writeBase64();
      Item item4 = Item::readBase64(s);
      EXPECT_EQ(item1, item4);
    }
  }
}

TEST(TestUsage, TypeId) {
  Summary s1{1, 1, 1.0};
  Summary s2{2, 2, 2.0};
  Item i1{1, 1, 0.1};
  Item i2{2, 2, 0.2};
  Customer c1{"1"};
  Customer c2{"2"};

  EXPECT_EQ(s1.getTypeId(), s2.getTypeId());
  EXPECT_EQ(i1.getTypeId(), i2.getTypeId());
  EXPECT_EQ(c1.getTypeId(), c2.getTypeId());
  EXPECT_NE(s1.getTypeId(), i1.getTypeId());
  EXPECT_NE(s1.getTypeId(), c1.getTypeId());
  EXPECT_NE(i1.getTypeId(), c1.getTypeId());
}

TEST(TestUsage, CountInstances) {
#define EXPECT_COUNTS(s, i, c)                                                                                         \
  EXPECT_EQ(s, Summary::getInstanceCount());                                                                           \
  EXPECT_EQ(i, Item::getInstanceCount());                                                                              \
  EXPECT_EQ(c, Customer::getInstanceCount());

  EXPECT_COUNTS(0, 0, 0);
  {
    Summary s1{};
    EXPECT_COUNTS(1, 0, 0);
    {
      Summary s2{};
      EXPECT_COUNTS(2, 0, 0);
    }
    EXPECT_COUNTS(1, 0, 0);
    {
      Summary s3{};
      EXPECT_COUNTS(2, 0, 0);
      {
        Item i1{};
        EXPECT_COUNTS(2, 1, 0);
        {
          Item i2{};
          EXPECT_COUNTS(2, 2, 0);
          {
            Item i3{};
            EXPECT_COUNTS(2, 3, 0);
            {
              Customer c1{"1"};
              EXPECT_COUNTS(2, 3, 1);
            }
            EXPECT_COUNTS(2, 3, 0);
          }
          EXPECT_COUNTS(2, 2, 0);
        }
        EXPECT_COUNTS(2, 1, 0);
        {
          Customer c2{"2"};
          EXPECT_COUNTS(2, 1, 1);
          {
            Summary s4{};
            EXPECT_COUNTS(3, 1, 1);
          }
          EXPECT_COUNTS(2, 1, 1);
        }
        EXPECT_COUNTS(2, 1, 0);
      }
      EXPECT_COUNTS(2, 0, 0);
    }
    EXPECT_COUNTS(1, 0, 0);
  }
  EXPECT_COUNTS(0, 0, 0);

#undef EXPECT_COUNTS
}