#include "BitSet.hpp"
#include <bit>
#include <cassert>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace bitset {
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
bool getBit(const unsigned char* bytes, size_t index)
// Get the bit out of the bytes at the given index
{
    size_t byteIndex = index / 8;
    unsigned char bitIndex = index % 8;
    return bytes[byteIndex] & (1u << bitIndex);
}
//---------------------------------------------------------------------------
void setBit(unsigned char* bytes, size_t index, bool value)
// Set the bit to the given value at a specific index
{
    size_t byteIndex = index / 8;
    unsigned char bitIndex = index % 8;
    unsigned char& byte = bytes[byteIndex];
    if (value)
        byte |= 1u << bitIndex;
    else
        byte &= ~(1u << bitIndex);
}
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
BitSet::BitReference::BitReference(unsigned char* bytes, size_t bit)
    : bytes(bytes),
      bit(bit)
// Constructor
{
    assert(bytes);
}
//---------------------------------------------------------------------------
BitSet::BitReference::operator bool() const
// Convert to bool
{
    return getBit(bytes, bit);
}
//---------------------------------------------------------------------------
BitSet::BitReference& BitSet::BitReference::operator=(bool value)
// Set to bool
{
    assert(bytes);

    setBit(bytes, bit, value);

    return *this;
}
//---------------------------------------------------------------------------
BitSet::BitReference& BitSet::BitReference::operator=(const BitReference& other)
// Assignment
{
    return operator=(static_cast<bool>(other));
}
//---------------------------------------------------------------------------
BitSet::BitIterator::BitIterator()
    : bytes(nullptr),
      element(0)
// Constructor
{
}
//---------------------------------------------------------------------------
BitSet::BitIterator::BitIterator(unsigned char* bytes, size_t element)
    : bytes(bytes),
      element(element)
// Constructor
{
    assert(bytes);
}
//---------------------------------------------------------------------------
bool BitSet::BitIterator::operator==(const BitIterator& other) const
// Comparison
{
    return (bytes == other.bytes) && (element == other.element);
}
//---------------------------------------------------------------------------
BitSet::BitIterator::reference BitSet::BitIterator::operator*() const
// Dereference
{
    return {bytes, element};
}
//---------------------------------------------------------------------------
BitSet::BitIterator& BitSet::BitIterator::operator++()
// Pre-increment
{
    element++;
    return *this;
}
//---------------------------------------------------------------------------
BitSet::BitIterator BitSet::BitIterator::operator++(int)
// Post-increment
{
    BitIterator prev = *this;
    element++;
    return prev;
}
//---------------------------------------------------------------------------
BitSet::BitIterator& BitSet::BitIterator::operator--()
// Pre-increment
{
    assert(element > 0);

    element--;
    return *this;
}
//---------------------------------------------------------------------------
BitSet::BitIterator BitSet::BitIterator::operator--(int)
// Post-increment
{
    assert(element > 0);

    BitIterator prev = *this;
    element--;
    return prev;
}
//---------------------------------------------------------------------------
BitSet::ConstBitIterator::ConstBitIterator()
    : bytes(nullptr),
      element(0)
// Constructor
{
}
//---------------------------------------------------------------------------
BitSet::ConstBitIterator::ConstBitIterator(const unsigned char* bytes, size_t element)
    : bytes(bytes),
      element(element)
// Constructor
{
    assert(bytes);
}
//---------------------------------------------------------------------------
bool BitSet::ConstBitIterator::operator==(const ConstBitIterator& other) const
// Comparison
{
    return (bytes == other.bytes) && (element == other.element);
}
//---------------------------------------------------------------------------
BitSet::ConstBitIterator::reference BitSet::ConstBitIterator::operator*() const
// Dereference
{
    return getBit(bytes, element);
}
//---------------------------------------------------------------------------
BitSet::ConstBitIterator& BitSet::ConstBitIterator::operator++()
// Pre-increment
{
    element++;
    return *this;
}
//---------------------------------------------------------------------------
BitSet::ConstBitIterator BitSet::ConstBitIterator::operator++(int)
// Post-increment
{
    ConstBitIterator prev = *this;
    element++;
    return prev;
}
//---------------------------------------------------------------------------
BitSet::ConstBitIterator& BitSet::ConstBitIterator::operator--()
// Pre-increment
{
    assert(element > 0);

    element--;
    return *this;
}
//---------------------------------------------------------------------------
BitSet::ConstBitIterator BitSet::ConstBitIterator::operator--(int)
// Post-increment
{
    assert(element > 0);

    ConstBitIterator prev = *this;
    element--;
    return prev;
}
//---------------------------------------------------------------------------
BitSet::BitSet(size_t size)
    : elements(size),
      bytes((size + 7) / 8)
// Constructor
{
}
//---------------------------------------------------------------------------
size_t BitSet::size() const
// Return the number of bits in the bitset
{
    return elements;
}
//---------------------------------------------------------------------------
size_t BitSet::cardinality() const
// Return the number of bits set to true
{
    size_t result = 0;

    for (unsigned char byte : bytes)
        result += popcount(byte);

    return result;
}
//---------------------------------------------------------------------------
void BitSet::push_back(bool bit)
// Insert a bit at the end of the bit set
{
    if (elements == (8 * bytes.size()))
        bytes.push_back(0);

    operator[](elements++) = bit;
}
//---------------------------------------------------------------------------
BitSet::BitReference BitSet::front()
// Access the last bit
{
    assert(elements > 0);
    return operator[](0);
}
//---------------------------------------------------------------------------
bool BitSet::front() const
// Access the last bit
{
    assert(elements > 0);

    return operator[](0);
}
//---------------------------------------------------------------------------
BitSet::BitReference BitSet::back()
// Access the last bit
{
    assert(elements > 0);
    return operator[](elements - 1);
}
//---------------------------------------------------------------------------
bool BitSet::back() const
// Access the last bit
{
    assert(elements > 0);
    return operator[](elements - 1);
}
//---------------------------------------------------------------------------
BitSet::BitIterator BitSet::begin()
// Iterator pointing to the first element
{
    return {&bytes[0], 0};
}
//---------------------------------------------------------------------------
BitSet::BitIterator BitSet::end()
// Iterator pointing past the end
{
    return {&bytes[0], elements};
}
//---------------------------------------------------------------------------
BitSet::ConstBitIterator BitSet::begin() const
// Iterator pointing to the first element
{
    return {&bytes[0], 0};
}
//---------------------------------------------------------------------------
BitSet::ConstBitIterator BitSet::end() const
// Iterator pointing past the end
{
    return {&bytes[0], elements};
}
//---------------------------------------------------------------------------
BitSet::BitReference BitSet::operator[](size_t bit)
// Access a bit
{
    assert(bit < elements);

    return {&bytes[0], bit};
}
//---------------------------------------------------------------------------
bool BitSet::operator[](size_t bit) const
// Access a bit
{
    assert(bit < elements);

    return getBit(&bytes[0], bit);
}
//---------------------------------------------------------------------------
} // namespace bitset
//---------------------------------------------------------------------------
