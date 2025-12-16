#include "aegis/ui/scene_hash.hpp"

#include <array>
#include <cassert>

#include "aegis/ui/scene_builder.hpp"

// Test that hash_scene is constexpr-compatible with constexpr Scene
void test_hash_constexpr_compatibility() {
    // While we can't make a fully constexpr Scene due to std::span limitations,
    // hash_scene itself should be constexpr-capable
    constexpr aegis::ui::NodeHandle handles[] = {{0}, {1}};
    const std::span<const aegis::ui::NodeHandle> roots{handles, 2};
    const aegis::ui::Scene scene{roots};
    
    const std::uint64_t hash = aegis::ui::hash_scene(scene);
    assert(hash != 0); // Hash should be non-zero for non-empty scene
}

// Test determinism: same inputs produce same hash
void test_hash_determinism() {
    std::array<std::byte, 4096> buffer1{};
    aegis::ui::Arena arena1{buffer1};
    aegis::ui::SceneBuilder builder1{arena1};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        100.0f, 100.0f};
    
    const aegis::ui::NodeHandle root1 =
        builder1.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle roots1[] = {root1};
    const aegis::ui::Scene scene1 = builder1.finalize(std::span{roots1, 1});

    // Create identical scene in different arena
    std::array<std::byte, 4096> buffer2{};
    aegis::ui::Arena arena2{buffer2};
    aegis::ui::SceneBuilder builder2{arena2};

    const aegis::ui::NodeHandle root2 =
        builder2.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle roots2[] = {root2};
    const aegis::ui::Scene scene2 = builder2.finalize(std::span{roots2, 1});

    // Same logical structure should produce same hash
    const std::uint64_t hash1 = aegis::ui::hash_scene(scene1);
    const std::uint64_t hash2 = aegis::ui::hash_scene(scene2);
    
    assert(hash1 == hash2);
}

// Test memory address independence
void test_hash_memory_independence() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        100.0f, 100.0f};

    // Build scene 1
    {
        aegis::ui::SceneBuilder builder{arena};
        const aegis::ui::NodeHandle root =
            builder.add_node({42}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle roots[] = {root};
        const aegis::ui::Scene scene1 = builder.finalize(std::span{roots, 1});
        const std::uint64_t hash1 = aegis::ui::hash_scene(scene1);

        // Reset and rebuild - different memory addresses, same structure
        arena.reset();
        aegis::ui::SceneBuilder builder2{arena};
        const aegis::ui::NodeHandle root2 =
            builder2.add_node({42}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
        const aegis::ui::NodeHandle roots2[] = {root2};
        const aegis::ui::Scene scene2 = builder2.finalize(std::span{roots2, 1});
        const std::uint64_t hash2 = aegis::ui::hash_scene(scene2);

        // Despite different memory addresses, hashes should match
        assert(hash1 == hash2);
    }
}

// Test empty scene hash
void test_hash_empty_scene() {
    const std::span<const aegis::ui::NodeHandle> empty_roots{};
    const aegis::ui::Scene scene{empty_roots};
    
    const std::uint64_t hash = aegis::ui::hash_scene(scene);
    assert(hash != 0); // FNV-1a always produces non-zero hash
}

// Test single root hash
void test_hash_single_root() {
    const aegis::ui::NodeHandle root_handle{42};
    const std::span<const aegis::ui::NodeHandle> roots{&root_handle, 1};
    const aegis::ui::Scene scene{roots};
    
    const std::uint64_t hash = aegis::ui::hash_scene(scene);
    assert(hash != 0);
}

// Test multiple roots hash
void test_hash_multiple_roots() {
    const aegis::ui::NodeHandle root_handles[] = {{0}, {1}, {2}};
    const std::span<const aegis::ui::NodeHandle> roots{root_handles, 3};
    const aegis::ui::Scene scene{roots};
    
    const std::uint64_t hash = aegis::ui::hash_scene(scene);
    assert(hash != 0);
}

// Test different scenes produce different hashes
void test_hash_different_scenes() {
    // Scene 1: single root with handle 0
    const aegis::ui::NodeHandle roots1[] = {{0}};
    const aegis::ui::Scene scene1{std::span{roots1, 1}};
    const std::uint64_t hash1 = aegis::ui::hash_scene(scene1);

    // Scene 2: single root with handle 1 (different handle value)
    const aegis::ui::NodeHandle roots2[] = {{1}};
    const aegis::ui::Scene scene2{std::span{roots2, 1}};
    const std::uint64_t hash2 = aegis::ui::hash_scene(scene2);

    // Different scenes should (very likely) produce different hashes
    assert(hash1 != hash2);
}

// Test different root counts produce different hashes
void test_hash_different_root_counts() {
    const aegis::ui::NodeHandle handles[] = {{0}, {1}};
    
    // Scene 1: one root
    const aegis::ui::Scene scene1{std::span{handles, 1}};
    const std::uint64_t hash1 = aegis::ui::hash_scene(scene1);

    // Scene 2: two roots
    const aegis::ui::Scene scene2{std::span{handles, 2}};
    const std::uint64_t hash2 = aegis::ui::hash_scene(scene2);

    // Different root counts should produce different hashes
    assert(hash1 != hash2);
}

// Test hash with complex scene
void test_hash_complex_scene() {
    std::array<std::byte, 8192> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                        100.0f, 100.0f};

    // Build a scene with multiple nodes
    const aegis::ui::NodeHandle n1 =
        builder.add_node({1}, layout, {1}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle n2 =
        builder.add_node({2}, layout, {2}, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle n3 =
        builder.add_node({3}, layout, {3}, std::span<const aegis::ui::NodeHandle>{});

    const aegis::ui::NodeHandle roots[] = {n1, n2, n3};
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 3});

    const std::uint64_t hash = aegis::ui::hash_scene(scene);
    assert(hash != 0);

    // Verify determinism by computing hash again
    const std::uint64_t hash2 = aegis::ui::hash_scene(scene);
    assert(hash == hash2);
}

// Test root order matters for hash
void test_hash_root_order_matters() {
    const aegis::ui::NodeHandle handles[] = {{0}, {1}};
    
    // Scene 1: roots in order [0, 1]
    const aegis::ui::Scene scene1{std::span{handles, 2}};
    const std::uint64_t hash1 = aegis::ui::hash_scene(scene1);

    // Scene 2: roots in reverse order [1, 0]
    const aegis::ui::NodeHandle handles_reversed[] = {{1}, {0}};
    const aegis::ui::Scene scene2{std::span{handles_reversed, 2}};
    const std::uint64_t hash2 = aegis::ui::hash_scene(scene2);

    // Different order should produce different hashes
    assert(hash1 != hash2);
}

// Test repeated hashing produces same result
void test_hash_stability() {
    const aegis::ui::NodeHandle roots[] = {{5}, {10}, {15}};
    const aegis::ui::Scene scene{std::span{roots, 3}};
    
    // Hash multiple times
    const std::uint64_t hash1 = aegis::ui::hash_scene(scene);
    const std::uint64_t hash2 = aegis::ui::hash_scene(scene);
    const std::uint64_t hash3 = aegis::ui::hash_scene(scene);
    
    // All hashes should be identical
    assert(hash1 == hash2);
    assert(hash2 == hash3);
}

// Test hash works with SceneBuilder-created scenes
void test_hash_with_scene_builder() {
    std::array<std::byte, 4096> buffer{};
    aegis::ui::Arena arena{buffer};
    aegis::ui::SceneBuilder builder{arena};

    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Flex, aegis::ui::SizeMode::Flex,
                                        1.0f, 1.0f};

    // Create various node types
    const aegis::ui::NodeHandle text_node =
        builder.add_text_node({100}, layout, {1}, "Test text");
    const aegis::ui::NodeHandle grid_node =
        builder.add_grid_node({200}, layout, {2}, 2, std::span<const aegis::ui::NodeHandle>{});
    const aegis::ui::NodeHandle regular_node =
        builder.add_node({300}, layout, {3}, std::span<const aegis::ui::NodeHandle>{});

    const aegis::ui::NodeHandle roots[] = {text_node, grid_node, regular_node};
    const aegis::ui::Scene scene = builder.finalize(std::span{roots, 3});

    const std::uint64_t hash = aegis::ui::hash_scene(scene);
    assert(hash != 0);
}

int main() {
    test_hash_constexpr_compatibility();
    test_hash_determinism();
    test_hash_memory_independence();
    test_hash_empty_scene();
    test_hash_single_root();
    test_hash_multiple_roots();
    test_hash_different_scenes();
    test_hash_different_root_counts();
    test_hash_complex_scene();
    test_hash_root_order_matters();
    test_hash_stability();
    test_hash_with_scene_builder();
    
    return 0;
}
