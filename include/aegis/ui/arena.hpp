#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace aegis::ui {

/// Simple arena allocator for frame-local allocations
/// All allocations are freed together when the arena is reset
/// No per-allocation deallocation - memory is reclaimed on reset only
class Arena {
  public:
    /// Construct arena with pre-allocated buffer
    /// Buffer must outlive the Arena
    constexpr Arena(std::span<std::byte> buffer) noexcept : buffer_(buffer) {}

    /// Allocate memory from the arena
    /// Returns nullptr if allocation fails (insufficient space)
    /// No alignment guarantees beyond natural alignment
    [[nodiscard]] void* allocate(std::size_t size) noexcept {
        // Align to 8 bytes for safety
        constexpr std::size_t alignment = 8;
        const std::size_t aligned_offset = (offset_ + alignment - 1) & ~(alignment - 1);

        if (aligned_offset + size > buffer_.size()) {
            return nullptr; // Out of space
        }

        void* ptr = buffer_.data() + aligned_offset;
        offset_ = aligned_offset + size;
        return ptr;
    }

    /// Reset the arena, freeing all allocations
    /// Does not deallocate individual objects - caller responsible for cleanup
    constexpr void reset() noexcept { offset_ = 0; }

    /// Get current offset (bytes used)
    [[nodiscard]] constexpr std::size_t offset() const noexcept { return offset_; }

    /// Get total capacity
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return buffer_.size(); }

    /// Get remaining space
    [[nodiscard]] constexpr std::size_t remaining() const noexcept {
        return buffer_.size() - offset_;
    }

  private:
    std::span<std::byte> buffer_;
    std::size_t offset_{0};
};

} // namespace aegis::ui
