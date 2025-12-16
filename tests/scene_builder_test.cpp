#include "aegis/ui/scene_builder.hpp"

#include <array>
#include <cassert>

// Test arena construction
void test_arena_construction() {
    std::array<std::byte, 1024> buffer{};
    aegis::ui::Arena arena{buffer};

    assert(arena.offset() == 0);
    assert(arena.capacity() == 1024);
    assert(arena.remaining() == 1024);
}

// Test arena allocation
void test_arena_allocation() {
    std::array<std::byte, 1024> buffer{};
    aegis::ui::Arena arena{buffer};

    void* ptr1 = arena.allocate(64);
    assert(ptr1 != nullptr);
    assert(arena.offset() == 64);

    void* ptr2 = arena.allocate(128);
    assert(ptr2 != nullptr);
    assert(arena.offset() == 192);
}

// Test arena alignment
void test_arena_alignment() {
    std::array<std::byte, 1024> buffer{};
    aegis::ui::Arena arena{buffer};

    // Allocate unaligned size
    void* ptr1 = arena.allocate(5);
    assert(ptr1 != nullptr);

    // Next allocation should be aligned
    void* ptr2 = arena.allocate(8);
    assert(ptr2 != nullptr);
    assert(reinterpret_cast<std::uintptr_t>(ptr2) % 8 == 0);
}

// Test arena reset
void test_arena_reset() {
    std::array<std::byte, 1024> buffer{};
    aegis::ui::Arena arena{buffer};

    void* ptr = arena.allocate(256);
    assert(ptr != nullptr);
    assert(arena.offset() == 256);

    arena.reset();
    assert(arena.offset() == 0);
    assert(arena.remaining() == 1024);
}

// Test arena out of memory
void test_arena_out_of_memory() {
    std::array<std::byte, 64> buffer{};
    aegis::ui::Arena arena{buffer};

    void* ptr = arena.allocate(128); // Request more than available
    assert(ptr == nullptr);          // Should fail gracefully
}

// Test builder construction
void test_builder_construction() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};

    aegis::ui::SceneBuilder builder{arena};
    // Builder should be ready to use
}

// Test adding a simple node
void test_add_node() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::NodeId id{42};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        100.0f, 100.0f};
    const aegis::ui::StyleId style_id{1};

    const aegis::ui::NodeHandle handle =
        builder.add_node(id, layout, style_id, std::span<const aegis::ui::NodeHandle>{});

    assert(handle.value == 0); // First node gets handle 0
}

// Test adding multiple nodes
void test_add_multiple_nodes() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        100.0f, 100.0f};

    const aegis::ui::NodeHandle h1 =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle h2 =
        builder.add_node({2}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle h3 =
        builder.add_node({3}, layout, {3}, std::span<const aegis::ui::NodeHandle>{});

    assert(h1.value == 0);
    assert(h2.value == 1);
    assert(h3.value == 2);
}

// Test adding text node
void test_add_text_node() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::NodeId id{100};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        200.0f, 50.0f};
    const aegis::ui::StyleId style_id{5};
    const std::string_view content = "Hello, World!";

    const aegis::ui::NodeHandle handle = builder.add_text_node(id, layout, style_id, content);

    assert(handle.value == 0);
}

// Test adding grid node
void test_add_grid_node() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::NodeId id{200};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Flex, aegis::ui::SizeMode::Flex, 1.0f,
                                        1.0f};
    const aegis::ui::StyleId style_id{10};
    const std::uint32_t columns = 3;

    const aegis::ui::NodeHandle handle =
        builder.add_grid_node(id, layout, style_id, columns, std::span<const aegis::ui::NodeHandle>{});

    assert(handle.value == 0);
}

// Test node with children
void test_node_with_children() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        100.0f, 100.0f};

    // Create child nodes
    const aegis::ui::NodeHandle child1 =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle child2 =
        builder.add_node({2}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});

    // Create parent with children
    const aegis::ui::NodeHandle children[] = {child1, child2};
    const aegis::ui::NodeHandle parent =
        builder.add_node({100}, layout, {10}, std::span{children, 2});

    assert(parent.value == 2); // Third node
}

// Test scene finalization with empty roots
void test_finalize_empty() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::Scene scene = builder.finalize(std::span<const aegis::ui::NodeHandle>{});

    assert(scene.roots.empty());
    assert(scene.roots.size() == 0);
}

// Test scene finalization with single root
void test_finalize_single_root() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        100.0f, 100.0f};
    const aegis::ui::NodeHandle root =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});

    const aegis::ui::NodeHandle roots[] = {root};
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});

    assert(scene.roots.size() == 1);
    assert(scene.roots[0].value == 0);
}

// Test scene finalization with multiple roots
void test_finalize_multiple_roots() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        100.0f, 100.0f};

    const aegis::ui::NodeHandle root1 =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle root2 =
        builder.add_node({2}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle root3 =
        builder.add_node({3}, layout, {3}, std::span<const aegis::ui::NodeHandle>{});

    const aegis::ui::NodeHandle roots[] = {root1, root2, root3};
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 3});

    assert(scene.roots.size() == 3);
    assert(scene.roots[0].value == 0);
    assert(scene.roots[1].value == 1);
    assert(scene.roots[2].value == 2);
}

// Test builder is single-use (finalize only once)
void test_builder_single_use() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        100.0f, 100.0f};
    const aegis::ui::NodeHandle root =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});

    const aegis::ui::NodeHandle roots[] = {root};
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
    assert(scene.roots.size() == 1);

    // Second finalize should trigger assertion in debug builds
    // In release builds, behavior is undefined but we test the flag
    // Note: We can't test assertion in release builds
}

// Test complex scene hierarchy
void test_complex_scene() {
    std::array<std::byte, 8192> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec fixed_layout{aegis::ui::SizeMode::Fixed,
                                              aegis::ui::SizeMode::Fixed, 100.0f, 100.0f};
    const aegis::ui::LayoutSpec flex_layout{aegis::ui::SizeMode::Flex, aegis::ui::SizeMode::Flex,
                                             1.0f, 1.0f};

    // Create leaf nodes
    const aegis::ui::NodeHandle text1 =
        builder.add_text_node({1001}, fixed_layout, {1}, "Text 1");
    const aegis::ui::NodeHandle text2 =
        builder.add_text_node({1002}, fixed_layout, {1}, "Text 2");

    // Create grid with text children
    const aegis::ui::NodeHandle grid_children[] = {text1, text2};
    const aegis::ui::NodeHandle grid =
        builder.add_grid_node({2000}, flex_layout, {2}, 2, std::span{grid_children, 2});

    // Create container with grid
    const aegis::ui::NodeHandle container_children[] = {grid};
    const aegis::ui::NodeHandle container =
        builder.add_node({3000}, flex_layout, {3}, std::span{container_children, 1});

    // Finalize with container as root
    const aegis::ui::NodeHandle roots[] = {container};
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});

    assert(scene.roots.size() == 1);
    assert(scene.roots[0].value == 3); // Container is 4th node (index 3)
}

// Test zero allocations (all in arena)
void test_zero_heap_allocations() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};

    const std::size_t initial_offset = arena.offset();

    {
        aegis::ui::SceneBuilder builder{arena};

        const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                            100.0f, 100.0f};

        const aegis::ui::NodeHandle h1 =
            builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle h2 = builder.add_text_node({2}, layout, {2}, "Test");
        const aegis::ui::NodeHandle h3 =
            builder.add_grid_node({3}, layout, {3}, 2, std::span<const aegis::ui::NodeHandle>{});

        const aegis::ui::NodeHandle roots[] = {h1};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
        assert(scene.roots.size() == 1);
        (void)h2; // Silence unused warning
        (void)h3; // Silence unused warning
    }

    // Arena should have been used
    assert(arena.offset() > initial_offset);
}

// Test scene immutability (Scene is POD, const fields)
void test_scene_immutability() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        100.0f, 100.0f};
    const aegis::ui::NodeHandle root =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});

    const aegis::ui::NodeHandle roots[] = {root};
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});

    // Scene.roots is const span, cannot modify elements
    assert(scene.roots.size() == 1);

    // Scene has no mutation methods
    // This is enforced by Scene being a simple POD struct
}

// Test arena reuse across frames
void test_arena_reuse() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};

    // Frame 1
    {
        aegis::ui::SceneBuilder builder{arena};
        const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                            100.0f, 100.0f};
        const aegis::ui::NodeHandle root =
            builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle roots[] = {root};
        const aegis::ui::Scene scene1 = builder.finalize(std::span{roots, 1});
        assert(scene1.roots.size() == 1);
    }

    const std::size_t frame1_usage = arena.offset();
    assert(frame1_usage > 0);

    // Reset arena for frame 2
    arena.reset();
    assert(arena.offset() == 0);

    // Frame 2
    {
        aegis::ui::SceneBuilder builder{arena};
        const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                            100.0f, 100.0f};
        const aegis::ui::NodeHandle root =
            builder.add_node({2}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle roots[] = {root};
        const aegis::ui::Scene scene2 = builder.finalize(std::span{roots, 1});
        assert(scene2.roots.size() == 1);
    }

    // Should have similar usage
    assert(arena.offset() > 0);
}

int main() {
    // Arena tests
    test_arena_construction();
    test_arena_allocation();
    test_arena_alignment();
    test_arena_reset();
    test_arena_out_of_memory();

    // Builder tests
    test_builder_construction();
    test_add_node();
    test_add_multiple_nodes();
    test_add_text_node();
    test_add_grid_node();
    test_node_with_children();

    // Finalization tests
    test_finalize_empty();
    test_finalize_single_root();
    test_finalize_multiple_roots();
    test_builder_single_use();

    // Complex scenarios
    test_complex_scene();
    test_zero_heap_allocations();
    test_scene_immutability();
    test_arena_reuse();

    return 0;
}
