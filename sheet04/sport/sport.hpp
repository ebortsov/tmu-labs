#pragma once

namespace sport {
/// @brief Sort pointers in both arrays based on the contents they point to.
/// @details Semantically, we treat the values like pairs (points, time) and sort them higher-points-first
/// and resolve ties in favor of lower time.
/// @param points_start The points array. Must not be null.
/// @param times_start The times array. Must not be null.
/// @param size Number of elements in both arrays.
void sort(const unsigned **points_start, const unsigned **times_start, unsigned size);
} // namespace sport
