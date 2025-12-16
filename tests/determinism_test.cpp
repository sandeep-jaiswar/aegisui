#include "aegis/ui/scene_builder.hpp"
#include "aegis/ui/scene_hash.hpp"
#include "aegis/ui/style.hpp"

#include <array>
#include <cassert>

// UI-DET-002: Replay & Equivalence Tests
// Verify deterministic replay across frames

// Test that identical scenes produce identical hashes across multiple runs
void test_identical_scenes_across_runs() {
    std::array<std::uint64_t, 5> hashes{};

    // Build the same scene 5 times and collect hashes
    for (std::size_t run = 0; run < 5; ++run) {
        std::array<std::byte, 4096> buffer{};
        aegis::ui::Arena arena{buffer};
        aegis::ui::SceneBuilder builder{arena};

        const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed,
                                           aegis::ui::SizeMode::Fixed, 100.0f, 100.0f};

        // Build identical scene structure
        const aegis::ui::NodeHandle n1 =
            builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle n2 =
            builder.add_node({2}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle n3 =
            builder.add_text_node({3}, layout, {3}, "Test");

        const aegis::ui::NodeHandle roots[] = {n1, n2, n3};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 3});

        hashes[run] = aegis::ui::hash_scene(scene);
    }

    // All hashes must be identical
    for (std::size_t i = 1; i < hashes.size(); ++i) {
        assert(hashes[i] == hashes[0]);
    }
}

// Test that node ordering is stable across frames
void test_node_ordering_stable() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};

    std::array<aegis::ui::NodeHandle, 3> handles_frame1{};
    std::array<aegis::ui::NodeHandle, 3> handles_frame2{};

    // Frame 1
    {
        aegis::ui::SceneBuilder builder{arena};
        const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed,
                                           aegis::ui::SizeMode::Fixed, 50.0f, 50.0f};

        handles_frame1[0] =
            builder.add_node({10}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
        handles_frame1[1] =
            builder.add_node({20}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});
        handles_frame1[2] =
            builder.add_node({30}, layout, {3}, std::span<const aegis::ui::NodeHandle>{});

        const aegis::ui::NodeHandle roots[] = {handles_frame1[0], handles_frame1[1],
                                                handles_frame1[2]};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 3});
        (void)scene;
    }

    // Reset arena for frame 2
    arena.reset();

    // Frame 2 - same nodes in same order
    {
        aegis::ui::SceneBuilder builder{arena};
        const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed,
                                           aegis::ui::SizeMode::Fixed, 50.0f, 50.0f};

        handles_frame2[0] =
            builder.add_node({10}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
        handles_frame2[1] =
            builder.add_node({20}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});
        handles_frame2[2] =
            builder.add_node({30}, layout, {3}, std::span<const aegis::ui::NodeHandle>{});

        const aegis::ui::NodeHandle roots[] = {handles_frame2[0], handles_frame2[1],
                                                handles_frame2[2]};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 3});
        (void)scene;
    }

    // Node handles should be identical (same order)
    for (std::size_t i = 0; i < 3; ++i) {
        assert(handles_frame1[i].value == handles_frame2[i].value);
    }
}

// Test that style resolution is stable
void test_style_resolution_stable() {
    const aegis::ui::Color color1{255, 0, 0, 255};
    const aegis::ui::Border border1{color1, 2};
    const aegis::ui::Padding padding1{10, 10, 10, 10};
    const aegis::ui::Style style1{color1, border1, padding1};

    const aegis::ui::Color color2{0, 255, 0, 255};
    const aegis::ui::Border border2{color2, 1};
    const aegis::ui::Padding padding2{5, 5, 5, 5};
    const aegis::ui::Style style2{color2, border2, padding2};

    const aegis::ui::StyleEntry entries[] = {
        {aegis::ui::StyleId{0}, style1},
        {aegis::ui::StyleId{1}, style2}
    };
    const aegis::ui::StyleSheet stylesheet{std::span{entries, 2}};

    // Lookup same style multiple times
    const aegis::ui::Style* s1_lookup1 = stylesheet.lookup({0});
    const aegis::ui::Style* s1_lookup2 = stylesheet.lookup({0});
    const aegis::ui::Style* s2_lookup1 = stylesheet.lookup({1});
    const aegis::ui::Style* s2_lookup2 = stylesheet.lookup({1});

    // Lookups should be stable and return same results
    assert(s1_lookup1 == s1_lookup2);
    assert(s2_lookup1 == s2_lookup2);
    assert(s1_lookup1 != s2_lookup1);

    // Values should match
    assert(s1_lookup1->background_color.r == 255);
    assert(s2_lookup1->background_color.g == 255);
}

// Test scene replay produces same hash
void test_scene_replay_same_hash() {
    std::array<std::byte, 8192> buffer{};
    aegis::ui::Arena arena{buffer};

    std::uint64_t hash1 = 0;
    std::uint64_t hash2 = 0;

    // Build scene first time
    {
        aegis::ui::SceneBuilder builder{arena};
        const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Flex,
                                           aegis::ui::SizeMode::Flex, 1.0f, 1.0f};

        const aegis::ui::NodeHandle child1 =
            builder.add_text_node({100}, layout, {1}, "Child 1");
        const aegis::ui::NodeHandle child2 =
            builder.add_text_node({101}, layout, {1}, "Child 2");

        const aegis::ui::NodeHandle children[] = {child1, child2};
        const aegis::ui::NodeHandle parent =
            builder.add_node({200}, layout, {2}, std::span{children, 2});

        const aegis::ui::NodeHandle roots[] = {parent};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});

        hash1 = aegis::ui::hash_scene(scene);
    }

    // Reset and replay
    arena.reset();

    // Build same scene second time
    {
        aegis::ui::SceneBuilder builder{arena};
        const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Flex,
                                           aegis::ui::SizeMode::Flex, 1.0f, 1.0f};

        const aegis::ui::NodeHandle child1 =
            builder.add_text_node({100}, layout, {1}, "Child 1");
        const aegis::ui::NodeHandle child2 =
            builder.add_text_node({101}, layout, {1}, "Child 2");

        const aegis::ui::NodeHandle children[] = {child1, child2};
        const aegis::ui::NodeHandle parent =
            builder.add_node({200}, layout, {2}, std::span{children, 2});

        const aegis::ui::NodeHandle roots[] = {parent};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});

        hash2 = aegis::ui::hash_scene(scene);
    }

    // Replayed scene must have identical hash
    assert(hash1 == hash2);
}

// Test complex hierarchies are deterministic
void test_complex_hierarchy_deterministic() {
    std::array<std::uint64_t, 3> hashes{};

    for (std::size_t run = 0; run < 3; ++run) {
        std::array<std::byte, 16384> buffer{};
        aegis::ui::Arena arena{buffer};
        aegis::ui::SceneBuilder builder{arena};

        const aegis::ui::LayoutSpec fixed_layout{aegis::ui::SizeMode::Fixed,
                                                 aegis::ui::SizeMode::Fixed, 100.0f, 100.0f};
        const aegis::ui::LayoutSpec flex_layout{aegis::ui::SizeMode::Flex,
                                                aegis::ui::SizeMode::Flex, 1.0f, 1.0f};

        // Build complex hierarchy: container > grid > text nodes
        const aegis::ui::NodeHandle text1 =
            builder.add_text_node({1001}, fixed_layout, {1}, "Text 1");
        const aegis::ui::NodeHandle text2 =
            builder.add_text_node({1002}, fixed_layout, {1}, "Text 2");
        const aegis::ui::NodeHandle text3 =
            builder.add_text_node({1003}, fixed_layout, {1}, "Text 3");
        const aegis::ui::NodeHandle text4 =
            builder.add_text_node({1004}, fixed_layout, {1}, "Text 4");

        const aegis::ui::NodeHandle grid_children[] = {text1, text2, text3, text4};
        const aegis::ui::NodeHandle grid =
            builder.add_grid_node({2000}, flex_layout, {2}, 2, std::span{grid_children, 4});

        const aegis::ui::NodeHandle label =
            builder.add_text_node({3000}, fixed_layout, {3}, "Header");

        const aegis::ui::NodeHandle container_children[] = {label, grid};
        const aegis::ui::NodeHandle container =
            builder.add_node({4000}, flex_layout, {4}, std::span{container_children, 2});

        const aegis::ui::NodeHandle roots[] = {container};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});

        hashes[run] = aegis::ui::hash_scene(scene);
    }

    // All runs must produce identical hash
    assert(hashes[0] == hashes[1]);
    assert(hashes[1] == hashes[2]);
}

// Test that different root orders produce different hashes
void test_different_node_order_different_hash() {
    std::array<std::byte, 4096> buffer1{};
    aegis::ui::Arena arena1{buffer1};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    // Build scene with three nodes, roots in order: A, B, C
    std::uint64_t hash1 = 0;
    {
        aegis::ui::SceneBuilder builder{arena1};
        const aegis::ui::NodeHandle a =
            builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle b =
            builder.add_node({2}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle c =
            builder.add_node({3}, layout, {3}, std::span<const aegis::ui::NodeHandle>{});

        const aegis::ui::NodeHandle roots[] = {a, b, c};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 3});
        hash1 = aegis::ui::hash_scene(scene);
    }

    // Build scene with same three nodes, but roots in different order: C, B, A
    std::array<std::byte, 4096> buffer2{};
    aegis::ui::Arena arena2{buffer2};
    std::uint64_t hash2 = 0;
    {
        aegis::ui::SceneBuilder builder{arena2};
        const aegis::ui::NodeHandle a =
            builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle b =
            builder.add_node({2}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle c =
            builder.add_node({3}, layout, {3}, std::span<const aegis::ui::NodeHandle>{});

        // Use different root order
        const aegis::ui::NodeHandle roots[] = {c, b, a};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 3});
        hash2 = aegis::ui::hash_scene(scene);
    }

    // Different root order should produce different hash
    assert(hash1 != hash2);
}

// Test frame-to-frame consistency with arena reset
void test_frame_to_frame_consistency() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    std::array<std::uint64_t, 10> frame_hashes{};

    // Simulate 10 frames with same scene structure
    for (std::size_t frame = 0; frame < 10; ++frame) {
        aegis::ui::SceneBuilder builder{arena};

        const aegis::ui::NodeHandle root =
            builder.add_node({42}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});

        const aegis::ui::NodeHandle roots[] = {root};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});

        frame_hashes[frame] = aegis::ui::hash_scene(scene);

        // Reset arena for next frame
        arena.reset();
    }

    // All frames must produce identical hash
    for (std::size_t i = 1; i < frame_hashes.size(); ++i) {
        assert(frame_hashes[i] == frame_hashes[0]);
    }
}

int main() {
    test_identical_scenes_across_runs();
    test_node_ordering_stable();
    test_style_resolution_stable();
    test_scene_replay_same_hash();
    test_complex_hierarchy_deterministic();
    test_different_node_order_different_hash();
    test_frame_to_frame_consistency();

    return 0;
}
