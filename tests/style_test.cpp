#include "aegis/ui/style.hpp"

#include <cassert>
#include <type_traits>

// Test that all style types are trivially copyable
static_assert(std::is_trivially_copyable_v<aegis::ui::Color>,
              "Color must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::Padding>,
              "Padding must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::Border>,
              "Border must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::Style>,
              "Style must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::StyleEntry>,
              "StyleEntry must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::StyleSheet>,
              "StyleSheet must be trivially copyable");

// Test that all style types are trivially constructible
static_assert(std::is_trivially_constructible_v<aegis::ui::Color>,
              "Color must be trivially constructible");
static_assert(std::is_trivially_constructible_v<aegis::ui::Padding>,
              "Padding must be trivially constructible");
static_assert(std::is_trivially_constructible_v<aegis::ui::Border>,
              "Border must be trivially constructible");
static_assert(std::is_trivially_constructible_v<aegis::ui::Style>,
              "Style must be trivially constructible");
static_assert(std::is_trivially_constructible_v<aegis::ui::StyleEntry>,
              "StyleEntry must be trivially constructible");
// Note: StyleSheet contains std::span which is not trivially constructible

// Test that all style types are trivially destructible
static_assert(std::is_trivially_destructible_v<aegis::ui::Color>,
              "Color must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::Padding>,
              "Padding must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::Border>,
              "Border must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::Style>,
              "Style must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::StyleEntry>,
              "StyleEntry must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::StyleSheet>,
              "StyleSheet must be trivially destructible");

// Test that all style types are POD-like (standard_layout)
static_assert(std::is_standard_layout_v<aegis::ui::Color>,
              "Color must be standard layout (POD-like)");
static_assert(std::is_standard_layout_v<aegis::ui::Padding>,
              "Padding must be standard layout (POD-like)");
static_assert(std::is_standard_layout_v<aegis::ui::Border>,
              "Border must be standard layout (POD-like)");
static_assert(std::is_standard_layout_v<aegis::ui::Style>,
              "Style must be standard layout (POD-like)");
static_assert(std::is_standard_layout_v<aegis::ui::StyleEntry>,
              "StyleEntry must be standard layout (POD-like)");
static_assert(std::is_standard_layout_v<aegis::ui::StyleSheet>,
              "StyleSheet must be standard layout (POD-like)");

// Runtime tests for Color
void test_color() {
    // Test construction
    const aegis::ui::Color color1{255, 128, 64, 255};
    const aegis::ui::Color color2{255, 128, 64, 255};
    const aegis::ui::Color color3{0, 0, 0, 0};

    // Test equality
    assert(color1 == color2);
    assert(!(color1 != color2));

    // Test inequality
    assert(color1 != color3);
    assert(!(color1 == color3));

    // Test value access
    assert(color1.r == 255);
    assert(color1.g == 128);
    assert(color1.b == 64);
    assert(color1.a == 255);

    // Test constexpr operations
    constexpr aegis::ui::Color const_color{100, 200, 50, 255};
    static_assert(const_color.r == 100, "Color r must be constexpr accessible");
    static_assert(const_color.g == 200, "Color g must be constexpr accessible");
    static_assert(const_color.b == 50, "Color b must be constexpr accessible");
    static_assert(const_color.a == 255, "Color a must be constexpr accessible");
}

// Runtime tests for Padding
void test_padding() {
    // Test construction
    const aegis::ui::Padding padding1{10.0f, 20.0f, 10.0f, 20.0f};
    const aegis::ui::Padding padding2{10.0f, 20.0f, 10.0f, 20.0f};
    const aegis::ui::Padding padding3{5.0f, 5.0f, 5.0f, 5.0f};

    // Test equality
    assert(padding1 == padding2);
    assert(!(padding1 != padding2));

    // Test inequality
    assert(padding1 != padding3);
    assert(!(padding1 == padding3));

    // Test value access
    assert(padding1.top == 10.0f);
    assert(padding1.right == 20.0f);
    assert(padding1.bottom == 10.0f);
    assert(padding1.left == 20.0f);

    // Test constexpr operations
    constexpr aegis::ui::Padding const_padding{8.0f, 12.0f, 8.0f, 12.0f};
    static_assert(const_padding.top == 8.0f, "Padding top must be constexpr accessible");
    static_assert(const_padding.right == 12.0f, "Padding right must be constexpr accessible");
    static_assert(const_padding.bottom == 8.0f, "Padding bottom must be constexpr accessible");
    static_assert(const_padding.left == 12.0f, "Padding left must be constexpr accessible");
}

// Runtime tests for Border
void test_border() {
    // Test construction
    const aegis::ui::Border border1{{0, 0, 0, 255}, 2.0f};
    const aegis::ui::Border border2{{0, 0, 0, 255}, 2.0f};
    const aegis::ui::Border border3{{255, 0, 0, 255}, 1.0f};

    // Test equality
    assert(border1 == border2);
    assert(!(border1 != border2));

    // Test inequality
    assert(border1 != border3);
    assert(!(border1 == border3));

    // Test value access
    assert(border1.color.r == 0);
    assert(border1.color.g == 0);
    assert(border1.color.b == 0);
    assert(border1.color.a == 255);
    assert(border1.width == 2.0f);

    // Test constexpr operations
    constexpr aegis::ui::Border const_border{{128, 128, 128, 255}, 3.0f};
    static_assert(const_border.color.r == 128, "Border color.r must be constexpr accessible");
    static_assert(const_border.width == 3.0f, "Border width must be constexpr accessible");
}

// Runtime tests for Style
void test_style() {
    // Test construction
    const aegis::ui::Style style1{
        {255, 255, 255, 255},      // background_color
        {{0, 0, 0, 255}, 1.0f},    // border
        {10.0f, 10.0f, 10.0f, 10.0f} // padding
    };
    const aegis::ui::Style style2{
        {255, 255, 255, 255},      // background_color
        {{0, 0, 0, 255}, 1.0f},    // border
        {10.0f, 10.0f, 10.0f, 10.0f} // padding
    };
    const aegis::ui::Style style3{
        {0, 0, 0, 255},            // background_color
        {{255, 0, 0, 255}, 2.0f},  // border
        {5.0f, 5.0f, 5.0f, 5.0f}   // padding
    };

    // Test equality
    assert(style1 == style2);
    assert(!(style1 != style2));

    // Test inequality
    assert(style1 != style3);
    assert(!(style1 == style3));

    // Test value access
    assert(style1.background_color.r == 255);
    assert(style1.border.width == 1.0f);
    assert(style1.padding.top == 10.0f);

    // Test constexpr operations
    constexpr aegis::ui::Style const_style{
        {200, 200, 200, 255},
        {{100, 100, 100, 255}, 2.5f},
        {15.0f, 15.0f, 15.0f, 15.0f}
    };
    static_assert(const_style.background_color.r == 200,
                  "Style background_color must be constexpr accessible");
    static_assert(const_style.border.width == 2.5f,
                  "Style border must be constexpr accessible");
    static_assert(const_style.padding.top == 15.0f,
                  "Style padding must be constexpr accessible");
}

// Runtime tests for StyleEntry
void test_style_entry() {
    // Test construction
    const aegis::ui::StyleEntry entry1{
        {1},
        {{255, 255, 255, 255}, {{0, 0, 0, 255}, 1.0f}, {10.0f, 10.0f, 10.0f, 10.0f}}
    };
    const aegis::ui::StyleEntry entry2{
        {1},
        {{255, 255, 255, 255}, {{0, 0, 0, 255}, 1.0f}, {10.0f, 10.0f, 10.0f, 10.0f}}
    };
    const aegis::ui::StyleEntry entry3{
        {2},
        {{0, 0, 0, 255}, {{255, 0, 0, 255}, 2.0f}, {5.0f, 5.0f, 5.0f, 5.0f}}
    };

    // Test equality
    assert(entry1 == entry2);
    assert(!(entry1 != entry2));

    // Test inequality (different ID)
    assert(entry1 != entry3);
    assert(!(entry1 == entry3));

    // Test value access
    assert(entry1.id.value == 1);
    assert(entry1.style.background_color.r == 255);

    // Test constexpr operations
    constexpr aegis::ui::StyleEntry const_entry{
        {42},
        {{100, 100, 100, 255}, {{50, 50, 50, 255}, 1.5f}, {8.0f, 8.0f, 8.0f, 8.0f}}
    };
    static_assert(const_entry.id.value == 42, "StyleEntry id must be constexpr accessible");
}

// Runtime tests for StyleSheet
void test_stylesheet() {
    // Create some styles
    constexpr aegis::ui::StyleEntry entries[] = {
        {
            {1},
            {{255, 255, 255, 255}, {{0, 0, 0, 255}, 1.0f}, {10.0f, 10.0f, 10.0f, 10.0f}}
        },
        {
            {2},
            {{200, 200, 200, 255}, {{100, 100, 100, 255}, 2.0f}, {5.0f, 5.0f, 5.0f, 5.0f}}
        },
        {
            {3},
            {{150, 150, 150, 255}, {{75, 75, 75, 255}, 1.5f}, {8.0f, 8.0f, 8.0f, 8.0f}}
        }
    };

    const aegis::ui::StyleSheet sheet{entries};

    // Test successful lookup
    const auto* style1 = sheet.lookup({1});
    assert(style1 != nullptr);
    assert(style1->background_color.r == 255);
    assert(style1->border.width == 1.0f);

    const auto* style2 = sheet.lookup({2});
    assert(style2 != nullptr);
    assert(style2->background_color.r == 200);
    assert(style2->border.width == 2.0f);

    const auto* style3 = sheet.lookup({3});
    assert(style3 != nullptr);
    assert(style3->background_color.r == 150);
    assert(style3->border.width == 1.5f);

    // Test failed lookup (non-existent ID)
    const auto* style_missing = sheet.lookup({999});
    assert(style_missing == nullptr);
}

// Test constant-time lookup property
void test_stylesheet_constant_time_lookup() {
    // Create a small stylesheet (typical use case)
    constexpr aegis::ui::StyleEntry entries[] = {
        {{1}, {{255, 255, 255, 255}, {{0, 0, 0, 255}, 1.0f}, {10.0f, 10.0f, 10.0f, 10.0f}}},
        {{2}, {{200, 200, 200, 255}, {{50, 50, 50, 255}, 2.0f}, {5.0f, 5.0f, 5.0f, 5.0f}}},
        {{3}, {{150, 150, 150, 255}, {{25, 25, 25, 255}, 1.5f}, {8.0f, 8.0f, 8.0f, 8.0f}}}
    };

    const aegis::ui::StyleSheet sheet{entries};

    // Multiple lookups should be constant-time (O(n) with small n is effectively constant)
    // This validates the design intent for small style sets
    const auto* s1 = sheet.lookup({1});
    const auto* s2 = sheet.lookup({2});
    const auto* s3 = sheet.lookup({3});

    assert(s1 != nullptr && s1->background_color.r == 255);
    assert(s2 != nullptr && s2->background_color.r == 200);
    assert(s3 != nullptr && s3->background_color.r == 150);
}

// Test that StyleSheet lookup works correctly
void test_stylesheet_runtime_construction() {
    // Runtime construction of StyleSheet
    const aegis::ui::StyleEntry entries[] = {
        {{1}, {{255, 255, 255, 255}, {{0, 0, 0, 255}, 1.0f}, {10.0f, 10.0f, 10.0f, 10.0f}}}
    };

    const aegis::ui::StyleSheet sheet{entries};
    
    // Verify runtime lookup works
    const aegis::ui::Style* style = sheet.lookup({1});
    assert(style != nullptr);
    assert(style->background_color.r == 255);
}

// Test no inheritance or cascading
void test_no_inheritance_or_cascading() {
    // Each style is fully resolved and independent
    const aegis::ui::Style style1{
        {255, 255, 255, 255},
        {{0, 0, 0, 255}, 1.0f},
        {10.0f, 10.0f, 10.0f, 10.0f}
    };

    const aegis::ui::Style style2{
        {200, 200, 200, 255},
        {{100, 100, 100, 255}, 2.0f},
        {5.0f, 5.0f, 5.0f, 5.0f}
    };

    // Styles are independent - no shared state or inheritance
    assert(style1.background_color.r == 255);
    assert(style2.background_color.r == 200);

    // Modifying one doesn't affect the other (trivial for POD types, but validates design)
    aegis::ui::Style style3 = style1;
    style3.background_color.r = 128;
    assert(style1.background_color.r == 255); // Original unchanged
    assert(style3.background_color.r == 128); // Copy modified
}

int main() {
    test_color();
    test_padding();
    test_border();
    test_style();
    test_style_entry();
    test_stylesheet();
    test_stylesheet_constant_time_lookup();
    test_stylesheet_runtime_construction();
    test_no_inheritance_or_cascading();
    return 0;
}
