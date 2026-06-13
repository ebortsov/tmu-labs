#pragma once

namespace stockpiles {
/// @brief A stockpile with a unique identifier and mutable bottles count.
struct Stockpile {
    /// @brief ID of the stockpile.
    unsigned id;
    /// @brief Pointer to the count of bottles in this stockpile.
    unsigned *bottles;
};

/// @brief Initialize the stockpile array to serve requests.
/// @param stock_start The stockpile array. Must not be null.
/// @param stock_len Number of elements in the stockpile array.
void init(Stockpile *stock_start, unsigned stock_len);

/// @brief Serve a single request from a previously initialized stockpile array.
/// @param stock_start The stockpile array. Guaranteed to have been `init()` before.
/// @param stock_len Number of elements in the stockpile array.
/// @param amount Number of bottles to serve.
/// @param which Out parameter. Will be set to the stockpile
/// from which the request was served if the return value is true.
/// @return True if the request was served. False if no stockpile contains `amount` bottles.
bool serve(Stockpile *stock_start, unsigned stock_len, unsigned amount, Stockpile &which);
} // namespace stockpiles
