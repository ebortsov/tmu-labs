#pragma once

namespace disjoint_sets {
/// Union-find data structure
class DisjointSets {
public:
    /// Default constructor creating an empty structure.
    DisjointSets() = default;
    /// Create size single-element disjoint sets.

    /// The sets will be {0}, {1}, ..., {size - 1}.
    /// @param size Number of elements in the structure.
    explicit DisjointSets(unsigned size);

    /// Add a new set and return its representative.
    ///
    /// The set will contain a single element, which is the representative.
    /// @return The representative of the newly created set.
    unsigned add();
    /// Union two sets and return the new representative.
    ///
    /// The new representative will be one of the elements in the resulting set,
    /// but with no guarantee as to which.
    /// @param entry1 Any element in the first set.
    /// @param entry2 Any element in the second set.
    /// @return The representative of the union of the sets.
    unsigned unionSets(unsigned entry1, unsigned entry2);
    /// Find the representative of a set.
    ///
    /// The representative will be one of the elements in the set.
    /// This function is stable, i.e. calling `find` multiple times without intervening calls to `union`
    /// will always return the same representative for a given set.
    /// @param entry Any element in the set.
    /// @return The representative of the set.
    unsigned find(unsigned entry);
    /// Check if two elements are in separate sets.
    /// @param entry1 Any element.
    /// @param entry2 Any other element.
    /// @return True if entry1 and entry2 are in different sets; false otherwise.
    bool disjoint(unsigned entry1, unsigned entry2);
};
} // disjoint_sets
