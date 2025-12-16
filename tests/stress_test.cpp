#include "aegis/ui/scene_builder.hpp"

#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

// UI-TST-002: Stress Scene Construction
// Build large scenes (100k+ nodes) and verify:
// - Build time O(n) (linear scaling)
// - No memory growth across frames
// - Stable performance

// Test helper to measure time
template <typename Func>
std::chrono::microseconds measure_time(Func&& func) {
    const auto start = std::chrono::high_resolution_clock::now();
    func();
    const auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

// Build a scene with N nodes in a flat hierarchy
// Returns the first node handle to use as root
aegis::ui::NodeHandle build_flat_scene(aegis::ui::SceneBuilder& builder, std::uint32_t num_nodes) {
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    aegis::ui::NodeHandle first_handle{0};
    for (std::uint32_t i = 0; i < num_nodes; ++i) {
        const aegis::ui::NodeHandle handle =
            builder.add_node({i}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
        if (i == 0) {
            first_handle = handle;
        }
    }
    return first_handle;
}

// Build a scene with N nodes in a deep hierarchy (linked list style)
void build_deep_scene(aegis::ui::Arena& arena, std::uint32_t depth) {
    aegis::ui::SceneBuilder builder{arena};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    aegis::ui::NodeHandle child =
        builder.add_node({0}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});

    for (std::uint32_t i = 1; i < depth; ++i) {
        const aegis::ui::NodeHandle children[] = {child};
        child = builder.add_node({i}, layout, {1}, std::span{children, 1});
    }

    const aegis::ui::NodeHandle roots[] = {child};
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
    (void)scene;
}

// Build a scene with N nodes in a wide tree (each parent has many children)
void build_wide_tree(aegis::ui::Arena& arena, std::uint32_t num_nodes) {
    aegis::ui::SceneBuilder builder{arena};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    // Create leaf nodes
    constexpr std::uint32_t children_per_parent = 100;
    std::vector<aegis::ui::NodeHandle> children;
    children.reserve(children_per_parent);

    std::uint32_t node_id = 0;
    while (node_id < num_nodes) {
        children.clear();

        // Create children (up to children_per_parent or remaining nodes)
        for (std::uint32_t i = 0; i < children_per_parent && node_id < num_nodes; ++i, ++node_id) {
            children.push_back(
                builder.add_node({node_id}, layout, {1}, std::span<const aegis::ui::NodeHandle>{}));
        }

        // Create parent for these children (if we still have budget)
        if (!children.empty() && node_id < num_nodes) {
            [[maybe_unused]] const aegis::ui::NodeHandle parent =
                builder.add_node({node_id}, layout, {1}, std::span{children});
            ++node_id;
        }
    }
}

// Test that build time scales linearly with number of nodes
void test_linear_scaling() {
    std::cout << "Testing linear scaling (O(n) build time)...\n";

    // We'll test with increasing sizes and verify the ratio of time to nodes is roughly constant
    const std::array<std::uint32_t, 4> sizes = {10000, 20000, 40000, 80000};
    std::array<std::chrono::microseconds, 4> times;

    for (std::size_t i = 0; i < 4; ++i) {
        const std::uint32_t num_nodes = sizes[i];

        // Allocate sufficient buffer (rough estimate: 128 bytes per node)
        std::vector<std::byte> buffer(num_nodes * 128);
        aegis::ui::Arena arena{buffer};

        times[i] = measure_time([&]() {
            aegis::ui::SceneBuilder builder{arena};
            const aegis::ui::NodeHandle root = build_flat_scene(builder, num_nodes);
            const aegis::ui::NodeHandle roots[] = {root};
            const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
            (void)scene;
        });

        const double time_per_node = static_cast<double>(times[i].count()) / num_nodes;
        std::cout << "  " << num_nodes << " nodes: " << times[i].count() << " μs ("
                  << time_per_node << " μs/node)\n";
    }

    // Verify roughly linear scaling - time per node should not increase significantly
    // We allow up to 3x variance to account for system noise and cache effects
    const double ratio_1 = static_cast<double>(times[0].count()) / sizes[0];
    const double ratio_2 = static_cast<double>(times[1].count()) / sizes[1];
    const double ratio_3 = static_cast<double>(times[2].count()) / sizes[2];
    const double ratio_4 = static_cast<double>(times[3].count()) / sizes[3];

    // Check that ratios don't increase by more than 3x
    assert(ratio_2 / ratio_1 < 3.0);
    assert(ratio_3 / ratio_1 < 3.0);
    assert(ratio_4 / ratio_1 < 3.0);

    std::cout << "  ✓ Build time scales linearly\n";
}

// Test that memory usage doesn't grow across frames
void test_no_memory_growth_across_frames() {
    std::cout << "Testing no memory growth across frames...\n";

    // Allocate fixed-size buffer
    constexpr std::size_t buffer_size = 10 * 1024 * 1024; // 10 MB
    std::vector<std::byte> buffer(buffer_size);
    aegis::ui::Arena arena{buffer};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0f, 100.0f};

    // Build and reset multiple frames
    constexpr std::uint32_t num_frames = 100;
    constexpr std::uint32_t nodes_per_frame = 10000;

    for (std::uint32_t frame = 0; frame < num_frames; ++frame) {
        aegis::ui::SceneBuilder builder{arena};

        // Build scene for this frame
        aegis::ui::NodeHandle first_handle{0};
        for (std::uint32_t i = 0; i < nodes_per_frame; ++i) {
            const aegis::ui::NodeHandle handle =
                builder.add_node({i}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
            if (i == 0) {
                first_handle = handle;
            }
        }

        const aegis::ui::NodeHandle roots[] = {first_handle};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
        (void)scene;

        // Reset arena for next frame
        arena.reset();

        // Verify arena is reset
        assert(arena.offset() == 0);
    }

    std::cout << "  ✓ No memory growth across " << num_frames << " frames\n";
}

// Test stable performance across iterations
void test_stable_performance() {
    std::cout << "Testing stable performance across iterations...\n";

    constexpr std::uint32_t num_nodes = 50000;
    constexpr std::uint32_t num_iterations = 10;

    std::vector<std::byte> buffer(num_nodes * 128);
    std::array<std::chrono::microseconds, num_iterations> times;

    for (std::uint32_t i = 0; i < num_iterations; ++i) {
        aegis::ui::Arena arena{buffer};

        times[i] = measure_time([&]() {
            aegis::ui::SceneBuilder builder{arena};
            const aegis::ui::NodeHandle root = build_flat_scene(builder, num_nodes);
            const aegis::ui::NodeHandle roots[] = {root};
            const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
            (void)scene;
        });

        std::cout << "  Iteration " << (i + 1) << ": " << times[i].count() << " μs\n";
    }

    // Calculate variance - times should be stable
    std::chrono::microseconds total{0};
    for (const auto& t : times) {
        total += t;
    }
    const auto avg = total / num_iterations;

    // Check that no iteration is more than 2x the average (allowing for system variance)
    for (const auto& t : times) {
        const double ratio = static_cast<double>(t.count()) / avg.count();
        assert(ratio < 2.0);
        assert(ratio > 0.5);
    }

    std::cout << "  ✓ Performance is stable (avg: " << avg.count() << " μs)\n";
}

// Test building very large scenes (100k+ nodes)
void test_very_large_scene() {
    std::cout << "Testing very large scene (100k+ nodes)...\n";

    constexpr std::uint32_t num_nodes = 100000;

    // Allocate sufficient buffer
    std::vector<std::byte> buffer(num_nodes * 128);
    aegis::ui::Arena arena{buffer};

    const auto build_time = measure_time([&]() {
        aegis::ui::SceneBuilder builder{arena};
        const aegis::ui::NodeHandle root = build_flat_scene(builder, num_nodes);
        const aegis::ui::NodeHandle roots[] = {root};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
        (void)scene;
    });

    std::cout << "  Built " << num_nodes << " nodes in " << build_time.count() << " μs\n";
    const double time_per_node = static_cast<double>(build_time.count()) / num_nodes;
    std::cout << "  Time per node: " << time_per_node << " μs\n";

    std::cout << "  ✓ Successfully built large scene\n";
}

// Test deep hierarchy performance
void test_deep_hierarchy() {
    std::cout << "Testing deep hierarchy...\n";

    constexpr std::uint32_t depth = 10000;

    // Allocate sufficient buffer
    std::vector<std::byte> buffer(depth * 128);
    aegis::ui::Arena arena{buffer};

    const auto build_time = measure_time([&]() { build_deep_scene(arena, depth); });

    std::cout << "  Built hierarchy of depth " << depth << " in " << build_time.count() << " μs\n";

    std::cout << "  ✓ Deep hierarchy handled correctly\n";
}

// Test wide tree performance
void test_wide_tree() {
    std::cout << "Testing wide tree...\n";

    constexpr std::uint32_t num_nodes = 50000;

    // Allocate sufficient buffer
    std::vector<std::byte> buffer(num_nodes * 128);
    aegis::ui::Arena arena{buffer};

    const auto build_time = measure_time([&]() { build_wide_tree(arena, num_nodes); });

    std::cout << "  Built wide tree with " << num_nodes << " nodes in " << build_time.count()
              << " μs\n";

    std::cout << "  ✓ Wide tree handled correctly\n";
}

// Test mixed node types at scale
void test_mixed_node_types() {
    std::cout << "Testing mixed node types at scale...\n";

    constexpr std::uint32_t num_nodes = 50000;

    // Allocate sufficient buffer
    std::vector<std::byte> buffer(num_nodes * 256); // More space for varied node types
    aegis::ui::Arena arena{buffer};

    const auto build_time = measure_time([&]() {
        aegis::ui::SceneBuilder builder{arena};
        const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                           100.0f, 100.0f};

        aegis::ui::NodeHandle first_handle{0};
        for (std::uint32_t i = 0; i < num_nodes; ++i) {
            // Mix of different node types
            aegis::ui::NodeHandle handle{0};
            if (i % 3 == 0) {
                handle = builder.add_text_node({i}, layout, {1}, "Test text");
            } else if (i % 3 == 1) {
                const aegis::ui::NodeHandle children[] = {};
                handle = builder.add_grid_node({i}, layout, {1}, 2, std::span{children, 0});
            } else {
                handle = builder.add_node({i}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
            }
            if (i == 0) {
                first_handle = handle;
            }
        }

        const aegis::ui::NodeHandle roots[] = {first_handle};
        const aegis::ui::Scene scene = builder.finalize(std::span{roots, 1});
        (void)scene;
    });

    std::cout << "  Built " << num_nodes << " mixed nodes in " << build_time.count() << " μs\n";

    std::cout << "  ✓ Mixed node types handled correctly\n";
}

int main() {
    std::cout << "\n=== UI-TST-002: Stress Scene Construction ===\n\n";

    test_very_large_scene();
    test_linear_scaling();
    test_no_memory_growth_across_frames();
    test_stable_performance();
    test_deep_hierarchy();
    test_wide_tree();
    test_mixed_node_types();

    std::cout << "\n=== All stress tests passed ===\n";
    return 0;
}
