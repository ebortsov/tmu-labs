#include <atomic>

namespace bag {
template <typename T>
class InsertOnlyBag
{
public:
    struct Node
    {

        template <typename... Args>
        explicit Node(Args&&... args)
            : elem(std::forward<Args>(args)...), next(nullptr)
        {
        }
        T elem;
        // TODO: pointer to the next element
    };

    struct Iterator
    {
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        Iterator operator++(int) const
        {
            // TODO: advance iterator
        }
        Iterator& operator++()
        {
            // TODO: advance iterator
        }
        const T& operator*() const
        {
            return it->elem;
        }

        bool operator==(const Iterator& rhs) const = default;
        Node* it;
    };

    Iterator begin()
    {
        return /*TODO: iterator from the first element*/;
    }
    Iterator end()
    {
        return Iterator{};
    }

    Node* insert(T value)
    {
        return emplace(std::move(value));
    }

    template <typename... Args>
    Node* emplace(Args&&... args)
    {
        // TODO: insert a new node into the underlying list
    }

private:
    // TODO: some pointer pointing to the first element
};
} // namespace bag