#pragma once
#include <cstddef>
#include <memory>

namespace bump_alloc {
class RawBumpAllocator {
public:
  RawBumpAllocator(std::byte *start, size_t size);
  RawBumpAllocator(const RawBumpAllocator &) = delete;
  RawBumpAllocator &operator=(const RawBumpAllocator &) = delete;
  RawBumpAllocator(RawBumpAllocator &&) noexcept;
  RawBumpAllocator &operator=(RawBumpAllocator &&) noexcept;

  size_t allocated_bytes() const noexcept;
  std::span<std::byte> allocation_span() const noexcept;
  void clear();
};

template <typename T>
class BumpAllocator {
  explicit BumpAllocator(RawBumpAllocator *raw);
};
} // namespace bump_alloc