#pragma once
#include <string>
#include <string_view>

namespace config_eval {
// Concept required on the type returned from parse_config.
template <typename T>
concept Config = requires(const T &c, std::string_view key) {
  // Returns the number of keys in the config.
  { c.size() } -> std::same_as<size_t>;
  // Returns true if a given key is present in the Config; false otherwise.
  { c.has_key(key) } -> std::same_as<bool>;
  // Returns the value associated with a given key. Calling with a key not present in the config is undefined.
  { c[key] } -> std::convertible_to<std::string_view>;
  // Returns the value associated with a given key.
  // If the key is not present the behavior depends on whether the evaluation context is const.
  //   - in consteval context the value is an empty string
  //   - outside consteval it throws an std::out_of_range exception with the massagge
  { c.get(key) } -> std::convertible_to<std::string_view>;
  // has_key must be noexcept
  requires noexcept(c.has_key(key));
  // operator[] must be noexcept
  requires noexcept(c[key]);
};
} // namespace config_eval