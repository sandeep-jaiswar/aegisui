#include "aegis/ui/node.hpp"

#include <cassert>
#include <type_traits>

// Test that Node is trivially copyable
static_assert(std::is_trivially_copyable_v<aegis::ui::Node>,
              "Node must be trivially copyable");

// Test that Node is trivially destructible
static_assert(std::is_trivially_destructible_v<aegis::ui::Node>,
              "Node must be trivially destructible");

// Test that Node is standard layout
static_assert(std::is_standard_layout_v<aegis::ui::Node>, "Node must be standard layout");

// Test that Node has no virtual functions
static_assert(!std::is_polymorphic_v<aegis::ui::Node>, "Node must not have virtual functions");

// Runtime tests for Node construction
void test_node_construction() {
    // Test basic construction
    const aegis::ui::NodeId node_id{42};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Flex,
                                       100.0F, 1.0F};
    const aegis::ui::StyleId style_id{123};
    const aegis::ui::NodeHandle children_array[] = {{1}, {2}, {3}};
    const std::span<const aegis::ui::NodeHandle> children{children_array, 3};

    const aegis::ui::Node node{node_id, layout, style_id, children};

    // Verify fields
    assert(node.id == node_id);
    assert(node.layout == layout);
    assert(node.style_id == style_id);
    assert(node.children.data() == children.data());
    assert(node.children.size() == 3);
}

// Test Node with empty children span
void test_node_empty_children() {
    const aegis::ui::NodeId node_id{1};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Flex, aegis::ui::SizeMode::Flex, 1.0F,
                                       1.0F};
    const aegis::ui::StyleId style_id{10};
    const std::span<const aegis::ui::NodeHandle> empty_children{};

    const aegis::ui::Node node{node_id, layout, style_id, empty_children};

    assert(node.id.value == 1);
    assert(node.style_id.value == 10);
    assert(node.children.empty());
    assert(node.children.size() == 0);
}

// Test Node fields are accessible and match requirements
void test_node_fields() {
    const aegis::ui::NodeId node_id{99};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       200.0F, 150.0F};
    const aegis::ui::StyleId style_id{555};
    const std::span<const aegis::ui::NodeHandle> empty_children{};

    const aegis::ui::Node node{node_id, layout, style_id, empty_children};

    // Verify NodeId field
    assert(node.id.value == 99);

    // Verify LayoutSpec field
    assert(node.layout.width_mode == aegis::ui::SizeMode::Fixed);
    assert(node.layout.height_mode == aegis::ui::SizeMode::Fixed);
    assert(node.layout.width_value == 200.0F);
    assert(node.layout.height_value == 150.0F);

    // Verify StyleId field (not Style)
    assert(node.style_id.value == 555);

    // Verify children field (span with no ownership)
    assert(node.children.size() == 0);
}

// Test that Node uses StyleId, not Style
void test_node_uses_style_id() {
    // This test ensures we're using StyleId (a reference) not Style (data)
    const aegis::ui::StyleId style_id{42};
    const aegis::ui::Node node{aegis::ui::NodeId{1},
                               aegis::ui::LayoutSpec{aegis::ui::SizeMode::Flex,
                                                     aegis::ui::SizeMode::Flex, 1.0F, 1.0F},
                               style_id, std::span<const aegis::ui::NodeHandle>{}};

    // Verify it's a StyleId, which is a uint64_t value
    static_assert(std::is_same_v<decltype(aegis::ui::Node::style_id), aegis::ui::StyleId>,
                  "Node must contain StyleId");
    assert(node.style_id == style_id);
    assert(node.style_id.value == 42);
}

// Test no ownership ambiguity - spans don't own their data
void test_no_ownership_ambiguity() {
    const aegis::ui::NodeHandle children_storage[] = {{10}, {20}};
    const std::span<const aegis::ui::NodeHandle> children{children_storage, 2};

    const aegis::ui::Node node{aegis::ui::NodeId{1},
                               aegis::ui::LayoutSpec{aegis::ui::SizeMode::Fixed,
                                                     aegis::ui::SizeMode::Fixed, 100.0F, 100.0F},
                               aegis::ui::StyleId{1}, children};

    // Verify span points to external storage (no ownership)
    assert(node.children.data() == children_storage);
    assert(node.children.size() == 2);
    assert(node.children[0].value == 10);
    assert(node.children[1].value == 20);
}

int main() {
    test_node_construction();
    test_node_empty_children();
    test_node_fields();
    test_node_uses_style_id();
    test_no_ownership_ambiguity();
    return 0;
}
