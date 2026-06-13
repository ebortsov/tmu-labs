#pragma once
#include <cstddef>
#include <iterator>
#include <vector>
//---------------------------------------------------------------------------
namespace bitset {
//---------------------------------------------------------------------------
/// A bitset
class BitSet
{
private:
    /// The number of elements in the bitset
    size_t elements = 0;
    /// The storage for the bitset (keep the name as it is used during testing)
    std::vector<unsigned char> bytes;
    /// Friend declaration for one of the tests
    friend class TestBitSet_correctVectorUsage_Test;

public:
    /// A read-write adapter for an individual bit
    class BitReference
    {
        // more stuff goes here...
    };

    /// A bit iterator
    class BitIterator
    {
        // more stuff goes here...
    };

    /// A bit iterator
    class ConstBitIterator
    {
        // more stuff goes here...
    };

    /// Constructor
    BitSet() = default;
    /// Constructor
    explicit BitSet(size_t size);

    /// Return the number of bits in the bitset
    size_t size() const;
    /// Return the number of bits set to true
    size_t cardinality() const;

    /// Insert a bit at the end of the bit set
    void push_back(bool bit);

    /// Access the first bit
    BitReference front();
    /// Access the first bit
    bool front() const;
    /// Access the last bit
    BitReference back();
    /// Access the last bit
    bool back() const;

    /// Iterator pointing to the first element
    BitIterator begin();
    /// Iterator pointing past the end
    BitIterator end();

    /// ConstIterator pointing to the first element
    ConstBitIterator begin() const;
    /// ConstIterator pointing past the end
    ConstBitIterator end() const;

    /// Access a bit
    BitReference operator[](size_t bit);
    /// Access a bit
    bool operator[](size_t bit) const;
};
//---------------------------------------------------------------------------
} // namespace bitset
//---------------------------------------------------------------------------