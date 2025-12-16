#include "aegis/ui/grid_node.hpp"

#include <cassert>
#include <type_traits>

// Test that GridNode is trivially copyable
static_assert(std::is_trivially_copyable_v<aegis::ui::GridNode>,
              "GridNode must be trivially copyable");

// Test that GridNode is trivially destructible
static_assert(std::is_trivially_destructible_v<aegis::ui::GridNode>,
              "GridNode must be trivially destructible");

// Test that GridNode is standard layout
static_assert(std::is_standard_layout_v<aegis::ui::GridNode>, "GridNode must be standard layout");

// Test that GridNode has no virtual functions
static_assert(!std::is_polymorphic_v<aegis::ui::GridNode>,
              "GridNode must not have virtual functions");

// Runtime tests for GridNode construction
void test_grid_node_construction() {
    // Test basic construction with explicit column count
    const aegis::ui::NodeId node_id{42};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       300.0F, 200.0F};
    const aegis::ui::StyleId style_id{123};
    const std::uint32_t columns = 3;
    const aegis::ui::NodeHandle children_array[] = {{1}, {2}, {3}, {4}, {5}, {6}};
    const std::span<const aegis::ui::NodeHandle> children{children_array, 6};

    const aegis::ui::GridNode grid_node{node_id, layout, style_id, columns, children};

    // Verify fields
    assert(grid_node.id == node_id);
    assert(grid_node.layout == layout);
    assert(grid_node.style_id == style_id);
    assert(grid_node.columns == 3);
    assert(grid_node.children.data() == children.data());
    assert(grid_node.children.size() == 6);
}

// Test GridNode with empty children span
void test_grid_node_empty_children() {
    const aegis::ui::NodeId node_id{1};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Flex, aegis::ui::SizeMode::Flex, 1.0F,
                                       1.0F};
    const aegis::ui::StyleId style_id{10};
    const std::uint32_t columns = 2;
    const std::span<const aegis::ui::NodeHandle> empty_children{};

    const aegis::ui::GridNode grid_node{node_id, layout, style_id, columns, empty_children};

    assert(grid_node.id.value == 1);
    assert(grid_node.style_id.value == 10);
    assert(grid_node.columns == 2);
    assert(grid_node.children.empty());
    assert(grid_node.children.size() == 0);
}

// Test GridNode fields are accessible and match requirements
void test_grid_node_fields() {
    const aegis::ui::NodeId node_id{99};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       400.0F, 300.0F};
    const aegis::ui::StyleId style_id{555};
    const std::uint32_t columns = 4;
    const aegis::ui::NodeHandle children_array[] = {{10}, {20}, {30}, {40}};
    const std::span<const aegis::ui::NodeHandle> children{children_array, 4};

    const aegis::ui::GridNode grid_node{node_id, layout, style_id, columns, children};

    // Verify NodeId field
    assert(grid_node.id.value == 99);

    // Verify LayoutSpec field
    assert(grid_node.layout.width_mode == aegis::ui::SizeMode::Fixed);
    assert(grid_node.layout.height_mode == aegis::ui::SizeMode::Fixed);
    assert(grid_node.layout.width_value == 400.0F);
    assert(grid_node.layout.height_value == 300.0F);

    // Verify StyleId field (not Style)
    assert(grid_node.style_id.value == 555);

    // Verify columns field (explicit column count)
    assert(grid_node.columns == 4);

    // Verify children field (span with no ownership)
    assert(grid_node.children.size() == 4);
    assert(grid_node.children[0].value == 10);
    assert(grid_node.children[3].value == 40);
}

// Test that GridNode uses StyleId, not Style
void test_grid_node_uses_style_id() {
    // This test ensures we're using StyleId (a reference) not Style (data)
    const aegis::ui::StyleId style_id{42};
    const std::uint32_t columns = 1;
    const aegis::ui::GridNode grid_node{
        aegis::ui::NodeId{1},
        aegis::ui::LayoutSpec{aegis::ui::SizeMode::Flex, aegis::ui::SizeMode::Flex, 1.0F, 1.0F},
        style_id, columns, std::span<const aegis::ui::NodeHandle>{}};

    // Verify it's a StyleId, which is a uint64_t value
    static_assert(std::is_same_v<decltype(aegis::ui::GridNode::style_id), aegis::ui::StyleId>,
                  "GridNode must contain StyleId");
    assert(grid_node.style_id == style_id);
    assert(grid_node.style_id.value == 42);
}

// Test no ownership ambiguity - spans don't own their data
void test_no_ownership_ambiguity() {
    const aegis::ui::NodeHandle children_storage[] = {{10}, {20}, {30}};
    const std::span<const aegis::ui::NodeHandle> children{children_storage, 3};

    const aegis::ui::GridNode grid_node{aegis::ui::NodeId{1},
                                        aegis::ui::LayoutSpec{aegis::ui::SizeMode::Fixed,
                                                              aegis::ui::SizeMode::Fixed, 100.0F,
                                                              100.0F},
                                        aegis::ui::StyleId{1}, 3, children};

    // Verify span points to external storage (no ownership)
    assert(grid_node.children.data() == children_storage);
    assert(grid_node.children.size() == 3);
    assert(grid_node.children[0].value == 10);
    assert(grid_node.children[1].value == 20);
    assert(grid_node.children[2].value == 30);
}

// Test explicit column count variations
void test_explicit_column_count() {
    const aegis::ui::NodeId node_id{1};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       200.0F, 200.0F};
    const aegis::ui::StyleId style_id{1};

    // Test single column grid
    {
        const std::uint32_t columns = 1;
        const aegis::ui::NodeHandle children_array[] = {{1}, {2}, {3}};
        const std::span<const aegis::ui::NodeHandle> children{children_array, 3};
        const aegis::ui::GridNode grid_node{node_id, layout, style_id, columns, children};

        assert(grid_node.columns == 1);
        assert(grid_node.children.size() == 3);
    }

    // Test multi-column grid
    {
        const std::uint32_t columns = 5;
        const aegis::ui::NodeHandle children_array[] = {{1}, {2}, {3}, {4}, {5},
                                                        {6}, {7}, {8}, {9}, {10}};
        const std::span<const aegis::ui::NodeHandle> children{children_array, 10};
        const aegis::ui::GridNode grid_node{node_id, layout, style_id, columns, children};

        assert(grid_node.columns == 5);
        assert(grid_node.children.size() == 10);
    }
}

// Test row-major ordering semantics
void test_row_major_ordering() {
    // This test documents the row-major ordering expectation
    // Children are ordered left-to-right, top-to-bottom
    const aegis::ui::NodeId node_id{100};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       300.0F, 300.0F};
    const aegis::ui::StyleId style_id{200};
    const std::uint32_t columns = 3;

    // 3x2 grid with row-major ordering:
    // [1, 2, 3]
    // [4, 5, 6]
    const aegis::ui::NodeHandle children_array[] = {{1}, {2}, {3}, {4}, {5}, {6}};
    const std::span<const aegis::ui::NodeHandle> children{children_array, 6};

    const aegis::ui::GridNode grid_node{node_id, layout, style_id, columns, children};

    // Verify row-major ordering preserved in children span
    assert(grid_node.columns == 3);
    assert(grid_node.children.size() == 6);
    // First row: indices 0, 1, 2
    assert(grid_node.children[0].value == 1);
    assert(grid_node.children[1].value == 2);
    assert(grid_node.children[2].value == 3);
    // Second row: indices 3, 4, 5
    assert(grid_node.children[3].value == 4);
    assert(grid_node.children[4].value == 5);
    assert(grid_node.children[5].value == 6);
}

// Test GridNode with partial last row
void test_partial_last_row() {
    // Test grid where last row is not full
    const aegis::ui::NodeId node_id{1};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       300.0F, 200.0F};
    const aegis::ui::StyleId style_id{1};
    const std::uint32_t columns = 3;

    // 3 columns with 5 children = 2 rows, last row has 2 cells
    const aegis::ui::NodeHandle children_array[] = {{1}, {2}, {3}, {4}, {5}};
    const std::span<const aegis::ui::NodeHandle> children{children_array, 5};

    const aegis::ui::GridNode grid_node{node_id, layout, style_id, columns, children};

    assert(grid_node.columns == 3);
    assert(grid_node.children.size() == 5);
    // No auto-placement logic - just verify structure is preserved
    assert(grid_node.children[4].value == 5);
}

int main() {
    test_grid_node_construction();
    test_grid_node_empty_children();
    test_grid_node_fields();
    test_grid_node_uses_style_id();
    test_no_ownership_ambiguity();
    test_explicit_column_count();
    test_row_major_ordering();
    test_partial_last_row();
    return 0;
}
