#include "aegis/ui/scene_builder.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <new>

// UI-DET-003: Allocation Guard Tests
// Ensure no allocations occur during scene build
//
// NOTE: This test uses global operator new/delete overrides to track allocations.
// This is safe because:
// 1. This is a single-threaded test executable
// 2. AllocationGuard is used in a scoped manner (RAII)
// 3. No multi-threading is used in this test
// The global state is intentionally simple for test purposes.

// Global allocation tracking
namespace {
std::size_t g_allocation_count = 0;
bool g_allocation_tracking_enabled = false;
} // namespace

// Override global operator new to track allocations
void* operator new(std::size_t size) {
    if (g_allocation_tracking_enabled) {
        ++g_allocation_count;
    }
    void* ptr = std::malloc(size);
    if (!ptr) {
        throw std::bad_alloc();
    }
    return ptr;
}

// Override global operator new[] to track allocations
void* operator new[](std::size_t size) {
    if (g_allocation_tracking_enabled) {
        ++g_allocation_count;
    }
    void* ptr = std::malloc(size);
    if (!ptr) {
        throw std::bad_alloc();
    }
    return ptr;
}

// Override global operator delete
void operator delete(void* ptr) noexcept {
    std::free(ptr);
}

// Override global operator delete with size parameter
void operator delete(void* ptr, std::size_t) noexcept {
    std::free(ptr);
}

// Override global operator delete[]
void operator delete[](void* ptr) noexcept {
    std::free(ptr);
}

// Override global operator delete[] with size parameter
void operator delete[](void* ptr, std::size_t) noexcept {
    std::free(ptr);
}

// RAII helper to enable/disable allocation tracking
class AllocationGuard {
  public:
    AllocationGuard() {
        g_allocation_count = 0;
        g_allocation_tracking_enabled = true;
    }

    ~AllocationGuard() { g_allocation_tracking_enabled = false; }

    [[nodiscard]] std::size_t allocation_count() const { return g_allocation_count; }
};

// Test that SceneBuilder construction doesn't allocate
void test_builder_construction_no_allocation() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};

    AllocationGuard guard;
    aegis::ui::SceneBuilder builder{arena};
    (void)builder;

    assert(guard.allocation_count() == 0);
}

// Test that adding a regular node doesn't heap allocate
void test_add_node_no_allocation() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    AllocationGuard guard;
    const aegis::ui::NodeHandle handle =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
    (void)handle;

    assert(guard.allocation_count() == 0);
}

// Test that adding a text node doesn't heap allocate
void test_add_text_node_no_allocation() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    AllocationGuard guard;
    const aegis::ui::NodeHandle handle = builder.add_text_node({1}, layout, {1}, "Test text");
    (void)handle;

    assert(guard.allocation_count() == 0);
}

// Test that adding a grid node doesn't heap allocate
void test_add_grid_node_no_allocation() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    AllocationGuard guard;
    const aegis::ui::NodeHandle handle =
        builder.add_grid_node({1}, layout, {1}, 2, std::span<const aegis::ui::NodeHandle>{});
    (void)handle;

    assert(guard.allocation_count() == 0);
}

// Test that adding multiple nodes doesn't heap allocate
void test_add_multiple_nodes_no_allocation() {
    std::array<std::byte, 8192> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    AllocationGuard guard;

    for (std::uint32_t i = 0; i < 100; ++i) {
        const aegis::ui::NodeHandle handle =
            builder.add_node({i}, layout, {i}, std::span<const aegis::ui::NodeHandle>{});
        (void)handle;
    }

    assert(guard.allocation_count() == 0);
}

// Test that finalize doesn't heap allocate
void test_finalize_no_allocation() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    const aegis::ui::NodeHandle root =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});

    const aegis::ui::NodeHandle roots[] = {root};

    AllocationGuard guard;
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
    (void)scene;

    assert(guard.allocation_count() == 0);
}

// Test that finalize with empty roots doesn't heap allocate
void test_finalize_empty_no_allocation() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    AllocationGuard guard;
    const aegis::ui::Scene scene = builder.finalize(std::span<const aegis::ui::NodeHandle>{});
    (void)scene;

    assert(guard.allocation_count() == 0);
}

// Test that finalize with multiple roots doesn't heap allocate
void test_finalize_multiple_roots_no_allocation() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    const aegis::ui::NodeHandle r1 =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle r2 =
        builder.add_node({2}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle r3 =
        builder.add_node({3}, layout, {3}, std::span<const aegis::ui::NodeHandle>{});

    const aegis::ui::NodeHandle roots[] = {r1, r2, r3};

    AllocationGuard guard;
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 3});
    (void)scene;

    assert(guard.allocation_count() == 0);
}

// Test complete scene build doesn't heap allocate
void test_complete_scene_build_no_allocation() {
    std::array<std::byte, 16384> buffer{};
    aegis::ui::Arena arena{buffer};

    AllocationGuard guard;

    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec fixed_layout{aegis::ui::SizeMode::Fixed,
                                             aegis::ui::SizeMode::Fixed, 100.0f, 100.0f};
    const aegis::ui::LayoutSpec flex_layout{aegis::ui::SizeMode::Flex, aegis::ui::SizeMode::Flex,
                                            1.0f, 1.0f};

    // Build complex scene
    const aegis::ui::NodeHandle text1 =
        builder.add_text_node({1001}, fixed_layout, {1}, "Text 1");
    const aegis::ui::NodeHandle text2 =
        builder.add_text_node({1002}, fixed_layout, {1}, "Text 2");

    const aegis::ui::NodeHandle grid_children[] = {text1, text2};
    const aegis::ui::NodeHandle grid =
        builder.add_grid_node({2000}, flex_layout, {2}, 2, std::span{grid_children, 2});

    const aegis::ui::NodeHandle container_children[] = {grid};
    const aegis::ui::NodeHandle container =
        builder.add_node({3000}, flex_layout, {3}, std::span{container_children, 1});

    const aegis::ui::NodeHandle roots[] = {container};
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
    (void)scene;

    assert(guard.allocation_count() == 0);
}

// Test node with children doesn't heap allocate
void test_node_with_children_no_allocation() {
    std::array<std::byte, 8192> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    // Create children first (outside guard)
    const aegis::ui::NodeHandle child1 =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle child2 =
        builder.add_node({2}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});

    const aegis::ui::NodeHandle children[] = {child1, child2};

    AllocationGuard guard;
    const aegis::ui::NodeHandle parent = builder.add_node({100}, layout, {10}, std::span{children, 2});
    (void)parent;

    assert(guard.allocation_count() == 0);
}

// Test arena reset doesn't heap allocate
void test_arena_reset_no_allocation() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};

    // Use some arena memory
    void* ptr = arena.allocate(256);
    assert(ptr != nullptr);

    AllocationGuard guard;
    arena.reset();

    assert(guard.allocation_count() == 0);
}

// Test multiple frames don't heap allocate
void test_multiple_frames_no_allocation() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    AllocationGuard guard;

    // Simulate 5 frames
    for (std::size_t frame = 0; frame < 5; ++frame) {
        aegis::ui::SceneBuilder builder{arena};

        const aegis::ui::NodeHandle root =
            builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});

        const aegis::ui::NodeHandle roots[] = {root};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
        (void)scene;

        arena.reset();
    }

    assert(guard.allocation_count() == 0);
}

int main() {
    test_builder_construction_no_allocation();
    test_add_node_no_allocation();
    test_add_text_node_no_allocation();
    test_add_grid_node_no_allocation();
    test_add_multiple_nodes_no_allocation();
    test_finalize_no_allocation();
    test_finalize_empty_no_allocation();
    test_finalize_multiple_roots_no_allocation();
    test_complete_scene_build_no_allocation();
    test_node_with_children_no_allocation();
    test_arena_reset_no_allocation();
    test_multiple_frames_no_allocation();

    return 0;
}
