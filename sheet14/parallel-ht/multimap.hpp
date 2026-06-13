#pragma once
//---------------------------------------------------------------------------
#include <cstdint>
#include <utility>
#include <vector>
//---------------------------------------------------------------------------
namespace hashtable {
//---------------------------------------------------------------------------
/// A multimap based on a chaining hash table
class MultiMap
{
public:
    /// Value type
    using value_type = std::pair<uint32_t, uint32_t>;

protected:
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

    /// The hash table
    std::vector<Node*> hashTable;
    /// The node allocator
    std::allocator<Node> nodeAllocator;

public:
    /// An iterator
    class Iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = MultiMap::value_type;
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

        friend class MultiMap;
        Iterator(Node** bucket, Node** end, Node* node);

    public:
        reference operator*();
        pointer operator->();
        Iterator& operator++();
        Iterator operator++(int);

        /// Equality comparison
        bool operator==(const Iterator& other) const;
    };

    explicit MultiMap(uint32_t bucketCount);
    ~MultiMap();

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