#include "parallel-multimap.hpp"
#include <cassert>
#include <mutex>
#include <new>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace hashtable {
//---------------------------------------------------------------------------
ParallelMultiMap::Node::Node(value_type value)
    : value(std::move(value))
// Constructor
{
}
//---------------------------------------------------------------------------
ParallelMultiMap::Iterator::Iterator(Node** bucket, Node** end, Node* node)
    : bucket(bucket),
      end(end),
      node(node)
// Constructor
{
    advance();
}
//---------------------------------------------------------------------------
void ParallelMultiMap::Iterator::advance()
// Advance to the next entry
{
    while (!node && (bucket != end)) {
        ++bucket;
        node = (bucket == end) ? nullptr : *bucket;
    }
}
//---------------------------------------------------------------------------
ParallelMultiMap::Iterator::reference ParallelMultiMap::Iterator::operator*()
// Dereference
{
    assert(bucket != end);
    return node->value;
}
//---------------------------------------------------------------------------
ParallelMultiMap::Iterator::pointer ParallelMultiMap::Iterator::operator->()
// Pointer to member
{
    assert(bucket != end);
    return &node->value;
}
//---------------------------------------------------------------------------
ParallelMultiMap::Iterator& ParallelMultiMap::Iterator::operator++()
// Pre-increment
{
    assert(bucket != end);
    node = node->next;

    advance();

    return *this;
}
//---------------------------------------------------------------------------
ParallelMultiMap::Iterator ParallelMultiMap::Iterator::operator++(int)
// Post-increment
{
    assert(bucket != end);

    Iterator it(*this);
    operator++();
    return it;
}
//---------------------------------------------------------------------------
bool ParallelMultiMap::Iterator::operator==(const Iterator& other) const
// Equality comparison
{
    return (bucket == other.bucket) && (end == other.end) && (node == other.node);
}
//---------------------------------------------------------------------------
ParallelMultiMap::ParallelMultiMap(uint32_t bucketCount)
    : hashTable(bucketCount)
// Constructor
{
}
//---------------------------------------------------------------------------
ParallelMultiMap::~ParallelMultiMap()
// Destructor
{
    for (auto* node : hashTable) {
        while (node) {
            auto* next = node->next;
            node->~Node();
            nodeAllocator.deallocate(node, 1);
            node = next;
        }
    }
}
//---------------------------------------------------------------------------
ParallelMultiMap::Iterator ParallelMultiMap::insert(value_type value)
// Insert a value
{
    auto hash = value.first;
    auto slot = hash % hashTable.size();

    auto* node = new (nodeAllocator.allocate(1)) Node(std::move(value));

    {
        unique_lock lock(mutex);
        node->next = hashTable[slot];
        hashTable[slot] = node;
    }

    return {&hashTable[slot], &hashTable[hashTable.size()], node};
}
//---------------------------------------------------------------------------
ParallelMultiMap::Iterator ParallelMultiMap::findFirst(uint32_t key)
// Find the first value with the specified key
{
    auto hash = key;
    auto slot = hash % hashTable.size();

    Node* node;
    {
        shared_lock lock(mutex);
        node = hashTable[slot];
    }

    while (node && (node->value.first != key))
        node = node->next;

    return node ? Iterator{&hashTable[slot], &hashTable[hashTable.size()], node} : end();
}
//---------------------------------------------------------------------------
ParallelMultiMap::Iterator ParallelMultiMap::findNext(Iterator& it)
// Find the next value with the specified key
{
    assert(it != end());

    auto key = it.node->value.first;
    auto* node = it.node->next;

    while (node && (node->value.first != key))
        node = node->next;

    return node ? Iterator(it.bucket, it.end, node) : end();
}
//---------------------------------------------------------------------------
ParallelMultiMap::Iterator ParallelMultiMap::begin()
// Return an iterator pointing to the first element
{
    return {&hashTable[0], &hashTable[hashTable.size()], hashTable[0]};
}
//---------------------------------------------------------------------------
ParallelMultiMap::Iterator ParallelMultiMap::end()
// Return an iterator pointing to the first element
{
    return {&hashTable[hashTable.size()], &hashTable[hashTable.size()], nullptr};
}
//---------------------------------------------------------------------------
} // namespace hashtable
//---------------------------------------------------------------------------