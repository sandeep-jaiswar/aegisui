#include "aegis/ui/text_node.hpp"

#include <cassert>
#include <type_traits>

// Test that TextNode is trivially copyable
static_assert(std::is_trivially_copyable_v<aegis::ui::TextNode>,
              "TextNode must be trivially copyable");

// Test that TextNode is trivially destructible
static_assert(std::is_trivially_destructible_v<aegis::ui::TextNode>,
              "TextNode must be trivially destructible");

// Test that TextNode is standard layout
static_assert(std::is_standard_layout_v<aegis::ui::TextNode>,
              "TextNode must be standard layout");

// Test that TextNode has no virtual functions
static_assert(!std::is_polymorphic_v<aegis::ui::TextNode>,
              "TextNode must not have virtual functions");

// Runtime tests for TextNode construction
void test_text_node_construction() {
    // Test basic construction
    const aegis::ui::NodeId node_id{42};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0F, 50.0F};
    const aegis::ui::StyleId style_id{123};
    const char* text_content = "Hello, AegisUI!";
    const std::string_view content{text_content};

    const aegis::ui::TextNode text_node{node_id, layout, style_id, content};

    // Verify fields
    assert(text_node.id == node_id);
    assert(text_node.layout == layout);
    assert(text_node.style_id == style_id);
    assert(text_node.content == content);
    assert(text_node.content.data() == text_content);
    assert(text_node.content == "Hello, AegisUI!");
}

// Test TextNode with empty content
void test_text_node_empty_content() {
    const aegis::ui::NodeId node_id{1};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Flex, aegis::ui::SizeMode::Flex, 1.0F,
                                       1.0F};
    const aegis::ui::StyleId style_id{10};
    const std::string_view empty_content{};

    const aegis::ui::TextNode text_node{node_id, layout, style_id, empty_content};

    assert(text_node.id.value == 1);
    assert(text_node.style_id.value == 10);
    assert(text_node.content.empty());
}

// Test TextNode fields are accessible and match requirements
void test_text_node_fields() {
    const aegis::ui::NodeId node_id{99};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       200.0F, 150.0F};
    const aegis::ui::StyleId style_id{555};
    const std::string_view content{"Test text content"};

    const aegis::ui::TextNode text_node{node_id, layout, style_id, content};

    // Verify NodeId field
    assert(text_node.id.value == 99);

    // Verify LayoutSpec field
    assert(text_node.layout.width_mode == aegis::ui::SizeMode::Fixed);
    assert(text_node.layout.height_mode == aegis::ui::SizeMode::Fixed);
    assert(text_node.layout.width_value == 200.0F);
    assert(text_node.layout.height_value == 150.0F);

    // Verify StyleId field (not Style)
    assert(text_node.style_id.value == 555);

    // Verify content field
    assert(text_node.content.size() == 17);
    assert(text_node.content == "Test text content");
}

// Test that TextNode uses StyleId, not Style
void test_text_node_uses_style_id() {
    // This test ensures we're using StyleId (a reference) not Style (data)
    const aegis::ui::StyleId style_id{42};
    const aegis::ui::TextNode text_node{aegis::ui::NodeId{1},
                                        aegis::ui::LayoutSpec{aegis::ui::SizeMode::Flex,
                                                              aegis::ui::SizeMode::Flex, 1.0F, 1.0F},
                                        style_id, std::string_view{"Text"}};

    // Verify it's a StyleId, which is a uint64_t value
    static_assert(std::is_same_v<decltype(aegis::ui::TextNode::style_id), aegis::ui::StyleId>,
                  "TextNode must contain StyleId");
    assert(text_node.style_id == style_id);
    assert(text_node.style_id.value == 42);
}

// Test no ownership ambiguity - string_view doesn't own its data
void test_no_ownership_ambiguity() {
    const char* content_storage = "External text storage";
    const std::string_view content{content_storage};

    const aegis::ui::TextNode text_node{aegis::ui::NodeId{1},
                                        aegis::ui::LayoutSpec{aegis::ui::SizeMode::Fixed,
                                                              aegis::ui::SizeMode::Fixed, 100.0F,
                                                              100.0F},
                                        aegis::ui::StyleId{1}, content};

    // Verify string_view points to external storage (no ownership)
    assert(text_node.content.data() == content_storage);
    assert(text_node.content.size() == 21);
    assert(text_node.content == "External text storage");
}

// Test that TextNode can be used with compile-time string literals
void test_text_node_with_literals() {
    constexpr aegis::ui::NodeId node_id{5};
    constexpr aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                           100.0F, 20.0F};
    constexpr aegis::ui::StyleId style_id{7};

    const aegis::ui::TextNode text_node{node_id, layout, style_id, "Literal text"};

    assert(text_node.content == "Literal text");
    assert(text_node.content.size() == 12);
}

// Test that content lifetime is external (no allocations)
void test_no_allocations_during_construction() {
    // This test verifies that TextNode construction doesn't allocate
    // by showing that content pointer is unchanged

    const char* external_content = "Externally managed content";
    const std::string_view content{external_content};

    const aegis::ui::TextNode text_node{aegis::ui::NodeId{100},
                                        aegis::ui::LayoutSpec{aegis::ui::SizeMode::Fixed,
                                                              aegis::ui::SizeMode::Fixed, 150.0F,
                                                              30.0F},
                                        aegis::ui::StyleId{200}, content};

    // Verify no allocation occurred - same pointer
    assert(text_node.content.data() == external_content);
    assert(text_node.content == "Externally managed content");
}

// Test TextNode equality and comparison
void test_text_node_comparison() {
    const aegis::ui::NodeId node_id{1};
    const aegis::ui::LayoutSpec layout{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                       100.0F, 50.0F};
    const aegis::ui::StyleId style_id{1};
    const std::string_view content{"Same content"};

    const aegis::ui::TextNode text_node1{node_id, layout, style_id, content};
    const aegis::ui::TextNode text_node2{node_id, layout, style_id, content};

    // TextNode doesn't define operator==, but fields can be compared individually
    assert(text_node1.id == text_node2.id);
    assert(text_node1.layout == text_node2.layout);
    assert(text_node1.style_id == text_node2.style_id);
    assert(text_node1.content == text_node2.content);
}

int main() {
    test_text_node_construction();
    test_text_node_empty_content();
    test_text_node_fields();
    test_text_node_uses_style_id();
    test_no_ownership_ambiguity();
    test_text_node_with_literals();
    test_no_allocations_during_construction();
    test_text_node_comparison();
    return 0;
}
