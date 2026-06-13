#include "../BitSet.hpp"
#include <gtest/gtest.h>
#include <iterator>
#include <type_traits>
//---------------------------------------------------------------------------
using namespace std;
namespace bitset {
//---------------------------------------------------------------------------
TEST(TestBitSet, assignment)
{
    BitSet bitset(16);
    const BitSet& constBitset = bitset;

    bitset.front() = true;
    bitset[8] = true;
    bitset.back() = true;

    ASSERT_TRUE(bitset.front());
    ASSERT_TRUE(bitset[8]);
    ASSERT_TRUE(bitset.back());
    ASSERT_TRUE(constBitset.front());
    ASSERT_TRUE(constBitset[8]);
    ASSERT_TRUE(constBitset.back());

    for (unsigned i = 1; i < 15; ++i)
        ASSERT_FALSE((i != 8) && bitset[i]);

    BitSet emptySet(0);
    ASSERT_EQ(emptySet.size(), 0);

    BitSet byteSet(8);
    ASSERT_EQ(byteSet.size(), 8);
    byteSet[7] = true;
    ASSERT_TRUE(byteSet[7]);
    ASSERT_TRUE(byteSet.back());
    byteSet.back() = false;
    ASSERT_FALSE(byteSet[7]);
    ASSERT_FALSE(byteSet.back());
}
//---------------------------------------------------------------------------
TEST(TestBitSet, assignmentFromReference)
{
    BitSet bitset(16);

    for (unsigned i = 0; i < 16; ++i)
        bitset[i] = ((i % 2) == 0);
    for (unsigned i = 0; i < 16; ++i)
        ASSERT_EQ(bitset[i], (i % 2) == 0);

    for (unsigned i = 1; i < 16; ++i)
        bitset[i - 1] = bitset[i];
    for (unsigned i = 0; i < 15; ++i)
        ASSERT_EQ(bitset[i], (i % 2) != 0);
    ASSERT_FALSE(bitset[15]);
}
//---------------------------------------------------------------------------
TEST(TestBitSet, pushBack)
{
    BitSet bitset;

    for (unsigned i = 0; i < 10; ++i) {
        bitset.push_back(i % 2);
        ASSERT_EQ(bitset.front(), false);
        ASSERT_EQ(bitset.back(), i % 2);
        ASSERT_EQ(bitset.size(), i + 1);
        ASSERT_EQ(bitset.cardinality(), (i + 1) / 2);
    }
}

TEST(TestBitSet, correctVectorUsage)
{
    BitSet bitset;
    ASSERT_TRUE((std::is_same_v<decltype(bitset.bytes), std::vector<unsigned char>>));
    ASSERT_TRUE(bitset.bytes.empty());
    bitset.push_back(true);
    ASSERT_EQ(bitset.bytes.size(), 1);
    bitset.push_back(true);
    bitset.push_back(true);
    bitset.push_back(true);
    bitset.push_back(true);
    bitset.push_back(true);
    bitset.push_back(true);
    bitset.push_back(true);
    ASSERT_EQ(bitset.bytes.size(), 1);
    bitset.push_back(true);
    ASSERT_EQ(bitset.bytes.size(), 2);
    ASSERT_EQ(sizeof(bitset), sizeof(std::vector<unsigned char>) + sizeof(size_t));
}
//---------------------------------------------------------------------------
TEST(TestBitSet, iteratorConcept)
{
    ASSERT_TRUE(bidirectional_iterator<BitSet::BitIterator>);
}
//---------------------------------------------------------------------------
TEST(TestBitSet, rangeFor)
{
    BitSet bitset;

    for (unsigned i = 0; i < 10; ++i)
        bitset.push_back(i % 2);

    unsigned i = 0;
    for (BitSet::BitReference bit : bitset) {
        ASSERT_EQ(bit, i % 2);
        ++i;
    }
}
//---------------------------------------------------------------------------
TEST(TestBitSet, constRangeFor)
{
    BitSet bitset;
    const BitSet& constBitset = bitset;

    for (unsigned i = 0; i < 10; ++i)
        bitset.push_back(i % 2);

    unsigned i = 0;
    for (bool bit : constBitset) {
        ASSERT_EQ(bit, i % 2);
        ++i;
    }
}
//---------------------------------------------------------------------------
TEST(TestBitSet, forwardIteration)
{
    BitSet bitset;

    for (unsigned i = 0; i < 10; ++i)
        bitset.push_back(i % 2);

    unsigned i = 0;
    for (BitSet::BitIterator it = bitset.begin(); it != bitset.end(); ++it, ++i)
        ASSERT_EQ(*it, i % 2);
}
//---------------------------------------------------------------------------
TEST(TestBitSet, forwardConstIteration)
{
    BitSet bitset;
    const BitSet& constBitset = bitset;

    for (unsigned i = 0; i < 10; ++i)
        bitset.push_back(i % 2);

    unsigned i = 0;
    for (BitSet::ConstBitIterator it = constBitset.begin(); it != constBitset.end(); ++it, ++i)
        ASSERT_EQ(*it, i % 2);
}
//---------------------------------------------------------------------------
TEST(TestBitSet, backwardIteration)
{
    BitSet bitset;

    for (unsigned i = 0; i < 10; ++i)
        bitset.push_back(i % 2);

    unsigned i = 10;
    BitSet::BitIterator it = bitset.end();
    do {
        it--;
        i--;
        ASSERT_EQ(*it, i % 2);
    } while (it != bitset.begin());
}
//---------------------------------------------------------------------------
TEST(TestBitSet, backwardConstIteration)
{
    BitSet bitset;
    const BitSet& constBitset = bitset;

    for (unsigned i = 0; i < 10; ++i)
        bitset.push_back(i % 2);

    unsigned i = 10;
    BitSet::ConstBitIterator it = constBitset.end();
    do {
        it--;
        i--;
        ASSERT_EQ(*it, i % 2);
    } while (it != constBitset.begin());
}
} // namespace bitset
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
