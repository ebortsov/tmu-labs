#pragma once
//---------------------------------------------------------------------------
#include <cstdint>
#include <shared_mutex>
#include <utility>
#include <vector>
//---------------------------------------------------------------------------
namespace hashtable {
//---------------------------------------------------------------------------
/// A multimap based on a chaining hash table
class ParallelMultiMap
{
public:
    /// Value type
    using value_type = std::pair<uint32_t, uint32_t>;

private:
    /// A hash table node
    struct Node
    {
        /// The value of the node
        value_type value = {};
        /// The next node in the chain
        Node* next = nullptr;

        /// Constructor
        Node() = default;
        /// Constructor
        explicit Node(value_type value);
    };

    /// Synchronization mutex
    mutable std::shared_mutex mutex;
    std::vector<Node*> hashTable;
    std::allocator<Node> nodeAllocator;

public:
    /// An iterator
    class Iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = ParallelMultiMap::value_type;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::forward_iterator_tag;

    private:
        /// The current bucket
        Node** bucket;
        /// The end of the bucket vector
        Node** end;
        /// The current node
        Node* node;

        /// Advance to the next entry
        void advance();

        friend class ParallelMultiMap;
        Iterator(Node** bucket, Node** end, Node* node);

    public:
        reference operator*();
        pointer operator->();
        Iterator& operator++();
        Iterator operator++(int);

        /// Equality comparison
        bool operator==(const Iterator& other) const;
    };

    explicit ParallelMultiMap(uint32_t bucketCount);
    ~ParallelMultiMap();

    /// Insert an entry
    Iterator insert(value_type value);
    /// Find the first entry with the specified key
    Iterator findFirst(uint32_t key);
    /// Find the next entry with the specified key (if any)
    Iterator findNext(Iterator& it);

    Iterator begin();
    Iterator end();
};
//---------------------------------------------------------------------------
} // namespace hashtable
//---------------------------------------------------------------------------