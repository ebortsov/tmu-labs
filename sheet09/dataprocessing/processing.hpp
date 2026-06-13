#include <memory>
#include <vector>

inline size_t hash_combine(size_t hash1, size_t hash2)
{
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash2 >> 2));
}

struct DataChunk
{
    /// @brief Column-wise storage of the data - each entry represents one column
    /// Example:
    ///     1|2|3
    ///     1|4|9
    ///     1|8|7
    /// stored:
    /// {{1, 1, 1}, {2, 4, 8}, {3, 9, 7}}
    std::vector<std::unique_ptr<int64_t[]>> columns;
    /// @brief number of rows - you can assume that no column is of different length
    size_t nr_rows;
};

/// @brief Apply the dataprocessing according to the description in the README
/// @param lhs first input chunk
/// @param rhs second input chunk
/// @param sort_merge_column columns to apply the first sorting and merging step
/// @param unique_column column to use for uniquing the rows
/// @param grouping_column grouped column
/// @param topN number of results to print
void process(const DataChunk& lhs, const DataChunk& rhs, size_t sort_merge_column, size_t unique_column, size_t grouping_column, size_t topN);