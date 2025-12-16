#include "aegis/ui/arena.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <type_traits>

// Test that Arena is trivially copyable (contains only std::span and size_t)
static_assert(std::is_trivially_copyable_v<aegis::ui::Arena>,
              "Arena must be trivially copyable");

// Test that Arena is trivially destructible (no cleanup needed)
static_assert(std::is_trivially_destructible_v<aegis::ui::Arena>,
              "Arena must be trivially destructible");

// Test basic allocation
void test_basic_allocation() {
    std::array<std::byte, 1024> buffer{};
    aegis::ui::Arena arena{buffer};

    // Allocate 64 bytes
    void* ptr1 = arena.allocate(64);
    assert(ptr1 != nullptr);
    assert(arena.offset() >= 64);
    assert(arena.remaining() <= 1024 - 64);

    // Allocate another 128 bytes
    void* ptr2 = arena.allocate(128);
    assert(ptr2 != nullptr);
    assert(ptr1 != ptr2);
    assert(arena.offset() >= 64 + 128);
}

// Test 8-byte alignment
void test_alignment() {
    std::array<std::byte, 1024> buffer{};
    aegis::ui::Arena arena{buffer};

    // Allocate 1 byte - should still be 8-byte aligned
    void* ptr1 = arena.allocate(1);
    assert(ptr1 != nullptr);
    assert(reinterpret_cast<std::uintptr_t>(ptr1) % 8 == 0);

    // Allocate 3 bytes - next allocation should still be 8-byte aligned
    void* ptr2 = arena.allocate(3);
    assert(ptr2 != nullptr);
    assert(reinterpret_cast<std::uintptr_t>(ptr2) % 8 == 0);

    // Allocate 7 bytes - next allocation should still be 8-byte aligned
    void* ptr3 = arena.allocate(7);
    assert(ptr3 != nullptr);
    assert(reinterpret_cast<std::uintptr_t>(ptr3) % 8 == 0);

    // Allocate 8 bytes - should be 8-byte aligned
    void* ptr4 = arena.allocate(8);
    assert(ptr4 != nullptr);
    assert(reinterpret_cast<std::uintptr_t>(ptr4) % 8 == 0);
}

// Test reset functionality
void test_reset() {
    std::array<std::byte, 1024> buffer{};
    aegis::ui::Arena arena{buffer};

    // Allocate some memory
    void* ptr1 = arena.allocate(256);
    assert(ptr1 != nullptr);
    const std::size_t offset_after_alloc = arena.offset();
    assert(offset_after_alloc > 0);

    // Reset the arena
    arena.reset();
    assert(arena.offset() == 0);
    assert(arena.remaining() == 1024);

    // Allocate again - should reuse the same space
    void* ptr2 = arena.allocate(256);
    assert(ptr2 != nullptr);
    assert(ptr2 == ptr1); // Same address after reset
}

// Test out-of-memory handling
void test_out_of_memory() {
    std::array<std::byte, 128> buffer{};
    aegis::ui::Arena arena{buffer};

    // Allocate most of the space
    void* ptr1 = arena.allocate(100);
    assert(ptr1 != nullptr);

    // Try to allocate more than remaining space
    void* ptr2 = arena.allocate(100);
    assert(ptr2 == nullptr); // Should fail

    // Small allocation should still work
    void* ptr3 = arena.allocate(8);
    assert(ptr3 != nullptr);
}

// Test capacity and remaining
void test_capacity_and_remaining() {
    std::array<std::byte, 2048> buffer{};
    aegis::ui::Arena arena{buffer};

    assert(arena.capacity() == 2048);
    assert(arena.remaining() == 2048);
    assert(arena.offset() == 0);

    // Allocate 512 bytes (may use more due to alignment)
    void* ptr = arena.allocate(512);
    assert(ptr != nullptr);

    assert(arena.capacity() == 2048); // Capacity never changes
    assert(arena.offset() > 0);
    assert(arena.remaining() == 2048 - arena.offset());
    assert(arena.offset() + arena.remaining() == 2048);
}

// Test multiple allocations
void test_multiple_allocations() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};

    constexpr std::size_t num_allocs = 100;
    void* ptrs[num_allocs];

    // Make many small allocations
    for (std::size_t i = 0; i < num_allocs; ++i) {
        ptrs[i] = arena.allocate(16);
        assert(ptrs[i] != nullptr);
        assert(reinterpret_cast<std::uintptr_t>(ptrs[i]) % 8 == 0);
    }

    // Verify all pointers are unique
    for (std::size_t i = 0; i < num_allocs; ++i) {
        for (std::size_t j = i + 1; j < num_allocs; ++j) {
            assert(ptrs[i] != ptrs[j]);
        }
    }
}

// Test constexpr operations
void test_constexpr_operations() {
    std::array<std::byte, 1024> buffer{};
    const aegis::ui::Arena arena{buffer};

    // These should all be constexpr-compatible
    constexpr std::size_t expected_capacity = 1024;
    assert(arena.capacity() == expected_capacity);
    assert(arena.offset() == 0);
    assert(arena.remaining() == expected_capacity);
}

// Test arena reuse across frames
void test_frame_reuse() {
    std::array<std::byte, 2048> buffer{};
    aegis::ui::Arena arena{buffer};

    // Simulate multiple frames
    for (std::size_t frame = 0; frame < 10; ++frame) {
        // Allocate some memory for this frame
        void* ptr1 = arena.allocate(128);
        void* ptr2 = arena.allocate(256);
        void* ptr3 = arena.allocate(64);

        assert(ptr1 != nullptr);
        assert(ptr2 != nullptr);
        assert(ptr3 != nullptr);

        // At end of frame, reset
        arena.reset();
        assert(arena.offset() == 0);
        assert(arena.remaining() == 2048);
    }
}

// Test zero-size allocation
void test_zero_size_allocation() {
    std::array<std::byte, 1024> buffer{};
    aegis::ui::Arena arena{buffer};

    // Allocate 0 bytes
    void* ptr = arena.allocate(0);
    // Implementation may return nullptr or valid pointer for 0-size allocation
    // Both behaviors are acceptable
    (void)ptr;
}

// Test large single allocation
void test_large_allocation() {
    std::array<std::byte, 8192> buffer{};
    aegis::ui::Arena arena{buffer};

    // Allocate most of the arena at once
    void* ptr = arena.allocate(8000);
    assert(ptr != nullptr);
    assert(reinterpret_cast<std::uintptr_t>(ptr) % 8 == 0);

    // Should have consumed at least 8000 bytes
    assert(arena.offset() >= 8000);
    // Should have less than 200 bytes remaining
    assert(arena.remaining() < 200);
}

int main() {
    test_basic_allocation();
    test_alignment();
    test_reset();
    test_out_of_memory();
    test_capacity_and_remaining();
    test_multiple_allocations();
    test_constexpr_operations();
    test_frame_reuse();
    test_zero_size_allocation();
    test_large_allocation();
    return 0;
}
