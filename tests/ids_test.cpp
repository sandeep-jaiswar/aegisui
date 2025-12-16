#include "aegis/ui/ids.hpp"

#include <cassert>
#include <type_traits>

// Test that all ID types are trivially copyable
static_assert(std::is_trivially_copyable_v<aegis::ui::NodeId>,
              "NodeId must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::StyleId>,
              "StyleId must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::NodeHandle>,
              "NodeHandle must be trivially copyable");

// Test that all ID types have no implicit constructors
static_assert(std::is_trivially_constructible_v<aegis::ui::NodeId>,
              "NodeId must be trivially constructible");
static_assert(std::is_trivially_constructible_v<aegis::ui::StyleId>,
              "StyleId must be trivially constructible");
static_assert(std::is_trivially_constructible_v<aegis::ui::NodeHandle>,
              "NodeHandle must be trivially constructible");

// Test that all ID types are trivially destructible
static_assert(std::is_trivially_destructible_v<aegis::ui::NodeId>,
              "NodeId must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::StyleId>,
              "StyleId must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::NodeHandle>,
              "NodeHandle must be trivially destructible");

// Runtime tests for NodeId
void test_node_id() {
    // Test construction
    const aegis::ui::NodeId id1{42};
    const aegis::ui::NodeId id2{42};
    const aegis::ui::NodeId id3{99};

    // Test equality
    assert(id1 == id2);
    assert(!(id1 != id2));

    // Test inequality
    assert(id1 != id3);
    assert(!(id1 == id3));

    // Test value access
    assert(id1.value == 42);
    assert(id3.value == 99);

    // Test constexpr operations
    constexpr aegis::ui::NodeId const_id{123};
    static_assert(const_id.value == 123, "NodeId value must be constexpr accessible");
}

// Runtime tests for StyleId
void test_style_id() {
    // Test construction
    const aegis::ui::StyleId id1{100};
    const aegis::ui::StyleId id2{100};
    const aegis::ui::StyleId id3{200};

    // Test equality
    assert(id1 == id2);
    assert(!(id1 != id2));

    // Test inequality
    assert(id1 != id3);
    assert(!(id1 == id3));

    // Test value access
    assert(id1.value == 100);
    assert(id3.value == 200);

    // Test constexpr operations
    constexpr aegis::ui::StyleId const_id{456};
    static_assert(const_id.value == 456, "StyleId value must be constexpr accessible");
}

// Runtime tests for NodeHandle
void test_node_handle() {
    // Test construction
    const aegis::ui::NodeHandle handle1{10};
    const aegis::ui::NodeHandle handle2{10};
    const aegis::ui::NodeHandle handle3{20};

    // Test equality
    assert(handle1 == handle2);
    assert(!(handle1 != handle2));

    // Test inequality
    assert(handle1 != handle3);
    assert(!(handle1 == handle3));

    // Test value access
    assert(handle1.value == 10);
    assert(handle3.value == 20);

    // Test constexpr operations
    constexpr aegis::ui::NodeHandle const_handle{789};
    static_assert(const_handle.value == 789, "NodeHandle value must be constexpr accessible");
}

// Test that there's no implicit generation logic
void test_no_implicit_generation() {
    // NodeId must be explicitly constructed
    const aegis::ui::NodeId id1{1};
    const aegis::ui::NodeId id2{2};

    // Values must be provided by caller, not auto-generated
    assert(id1.value == 1);
    assert(id2.value == 2);
    assert(id1 != id2); // Different values mean different IDs

    // Same for StyleId
    const aegis::ui::StyleId style1{10};
    const aegis::ui::StyleId style2{20};
    assert(style1.value == 10);
    assert(style2.value == 20);
    assert(style1 != style2);

    // Same for NodeHandle
    const aegis::ui::NodeHandle handle1{100};
    const aegis::ui::NodeHandle handle2{200};
    assert(handle1.value == 100);
    assert(handle2.value == 200);
    assert(handle1 != handle2);
}

int main() {
    test_node_id();
    test_style_id();
    test_node_handle();
    test_no_implicit_generation();
    return 0;
}
