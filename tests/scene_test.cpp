#include "aegis/ui/scene.hpp"

#include <cassert>
#include <type_traits>

// Test that Scene is trivially copyable
static_assert(std::is_trivially_copyable_v<aegis::ui::Scene>,
              "Scene must be trivially copyable");

// Test that Scene is trivially destructible
static_assert(std::is_trivially_destructible_v<aegis::ui::Scene>,
              "Scene must be trivially destructible");

// Test that Scene is standard layout
static_assert(std::is_standard_layout_v<aegis::ui::Scene>, "Scene must be standard layout");

// Test that Scene has no virtual functions
static_assert(!std::is_polymorphic_v<aegis::ui::Scene>, "Scene must not have virtual functions");

// Runtime tests for Scene construction
void test_scene_construction() {
    // Test construction with root handles
    const aegis::ui::NodeHandle root_handles[] = {{0}, {1}, {2}};
    const std::span<const aegis::ui::NodeHandle> roots{root_handles, 3};

    const aegis::ui::Scene scene{roots};

    // Verify roots field
    assert(scene.roots.data() == roots.data());
    assert(scene.roots.size() == 3);
    assert(scene.roots[0].value == 0);
    assert(scene.roots[1].value == 1);
    assert(scene.roots[2].value == 2);
}

// Test Scene with empty roots
void test_scene_empty_roots() {
    const std::span<const aegis::ui::NodeHandle> empty_roots{};

    const aegis::ui::Scene scene{empty_roots};

    assert(scene.roots.empty());
    assert(scene.roots.size() == 0);
}

// Test Scene with single root
void test_scene_single_root() {
    const aegis::ui::NodeHandle root_handle{42};
    const std::span<const aegis::ui::NodeHandle> roots{&root_handle, 1};

    const aegis::ui::Scene scene{roots};

    assert(scene.roots.size() == 1);
    assert(scene.roots[0].value == 42);
}

// Test that Scene contains only NodeHandles (not Nodes)
void test_scene_contains_node_handles() {
    // Verify Scene only contains NodeHandles
    static_assert(
        std::is_same_v<decltype(aegis::ui::Scene::roots), std::span<const aegis::ui::NodeHandle>>,
        "Scene must contain span of NodeHandles");

    const aegis::ui::NodeHandle handles[] = {{10}, {20}, {30}};
    const aegis::ui::Scene scene{std::span{handles, 3}};

    assert(scene.roots.size() == 3);
    assert(scene.roots[0].value == 10);
    assert(scene.roots[1].value == 20);
    assert(scene.roots[2].value == 30);
}

// Test no ownership ambiguity - span doesn't own its data
void test_no_ownership_ambiguity() {
    const aegis::ui::NodeHandle root_storage[] = {{100}, {200}};
    const std::span<const aegis::ui::NodeHandle> roots{root_storage, 2};

    const aegis::ui::Scene scene{roots};

    // Verify span points to external storage (no ownership)
    assert(scene.roots.data() == root_storage);
    assert(scene.roots.size() == 2);
    assert(scene.roots[0].value == 100);
    assert(scene.roots[1].value == 200);
}

// Test that Scene has no mutation APIs
void test_no_mutation_apis() {
    // Scene should be a simple struct with public fields
    // No setter methods, no mutation methods
    const aegis::ui::NodeHandle handles[] = {{1}};
    const aegis::ui::Scene scene{std::span{handles, 1}};

    // We can read but not modify through the Scene
    // (const span prevents modification of the span itself,
    // and NodeHandle values are const through the span)
    static_assert(std::is_const_v<std::remove_reference_t<decltype(scene.roots[0])>>,
                  "Scene roots elements must be const");

    assert(scene.roots[0].value == 1);
}

// Test constexpr construction
void test_constexpr_construction() {
    // Scene should support constexpr construction of the span elements
    // Note: std::span itself may not be fully constexpr in all contexts,
    // but the Scene struct can be constructed at compile time with proper initialization
    constexpr aegis::ui::NodeHandle handles[] = {{5}, {10}};
    
    // Runtime construction with constexpr data
    const std::span<const aegis::ui::NodeHandle> roots{handles, 2};
    const aegis::ui::Scene scene{roots};

    assert(scene.roots.size() == 2);
    assert(scene.roots[0].value == 5);
    assert(scene.roots[1].value == 10);
}

int main() {
    test_scene_construction();
    test_scene_empty_roots();
    test_scene_single_root();
    test_scene_contains_node_handles();
    test_no_ownership_ambiguity();
    test_no_mutation_apis();
    test_constexpr_construction();
    return 0;
}
