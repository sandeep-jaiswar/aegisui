#include "aegis/ui/layout.hpp"

#include <cassert>
#include <type_traits>

// Test that all layout types are trivially copyable
static_assert(std::is_trivially_copyable_v<aegis::ui::FixedSize>,
              "FixedSize must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::FlexSize>,
              "FlexSize must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::SizeMode>,
              "SizeMode must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::LayoutSpec>,
              "LayoutSpec must be trivially copyable");
static_assert(std::is_trivially_copyable_v<aegis::ui::LayoutIntent>,
              "LayoutIntent must be trivially copyable");

// Test that all layout types are trivially constructible
static_assert(std::is_trivially_constructible_v<aegis::ui::FixedSize>,
              "FixedSize must be trivially constructible");
static_assert(std::is_trivially_constructible_v<aegis::ui::FlexSize>,
              "FlexSize must be trivially constructible");
static_assert(std::is_trivially_constructible_v<aegis::ui::SizeMode>,
              "SizeMode must be trivially constructible");
static_assert(std::is_trivially_constructible_v<aegis::ui::LayoutSpec>,
              "LayoutSpec must be trivially constructible");
static_assert(std::is_trivially_constructible_v<aegis::ui::LayoutIntent>,
              "LayoutIntent must be trivially constructible");

// Test that all layout types are trivially destructible
static_assert(std::is_trivially_destructible_v<aegis::ui::FixedSize>,
              "FixedSize must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::FlexSize>,
              "FlexSize must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::SizeMode>,
              "SizeMode must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::LayoutSpec>,
              "LayoutSpec must be trivially destructible");
static_assert(std::is_trivially_destructible_v<aegis::ui::LayoutIntent>,
              "LayoutIntent must be trivially destructible");

// Test that all layout types are POD-like (standard_layout)
static_assert(std::is_standard_layout_v<aegis::ui::FixedSize>,
              "FixedSize must be standard layout (POD-like)");
static_assert(std::is_standard_layout_v<aegis::ui::FlexSize>,
              "FlexSize must be standard layout (POD-like)");
static_assert(std::is_standard_layout_v<aegis::ui::SizeMode>,
              "SizeMode must be standard layout (POD-like)");
static_assert(std::is_standard_layout_v<aegis::ui::LayoutSpec>,
              "LayoutSpec must be standard layout (POD-like)");
static_assert(std::is_standard_layout_v<aegis::ui::LayoutIntent>,
              "LayoutIntent must be standard layout (POD-like)");

// Runtime tests for FixedSize
void test_fixed_size() {
    // Test construction
    const aegis::ui::FixedSize size1{100.0f};
    const aegis::ui::FixedSize size2{100.0f};
    const aegis::ui::FixedSize size3{200.0f};

    // Test equality
    assert(size1 == size2);
    assert(!(size1 != size2));

    // Test inequality
    assert(size1 != size3);
    assert(!(size1 == size3));

    // Test value access
    assert(size1.pixels == 100.0f);
    assert(size3.pixels == 200.0f);

    // Test constexpr operations
    constexpr aegis::ui::FixedSize const_size{150.0f};
    static_assert(const_size.pixels == 150.0f, "FixedSize pixels must be constexpr accessible");
}

// Runtime tests for FlexSize
void test_flex_size() {
    // Test construction
    const aegis::ui::FlexSize flex1{1.0f};
    const aegis::ui::FlexSize flex2{1.0f};
    const aegis::ui::FlexSize flex3{2.0f};

    // Test equality
    assert(flex1 == flex2);
    assert(!(flex1 != flex2));

    // Test inequality
    assert(flex1 != flex3);
    assert(!(flex1 == flex3));

    // Test value access
    assert(flex1.weight == 1.0f);
    assert(flex3.weight == 2.0f);

    // Test constexpr operations
    constexpr aegis::ui::FlexSize const_flex{3.0f};
    static_assert(const_flex.weight == 3.0f, "FlexSize weight must be constexpr accessible");
}

// Runtime tests for SizeMode
void test_size_mode() {
    // Test enum values
    const aegis::ui::SizeMode fixed_mode = aegis::ui::SizeMode::Fixed;
    const aegis::ui::SizeMode flex_mode = aegis::ui::SizeMode::Flex;

    // Test inequality
    assert(fixed_mode != flex_mode);

    // Test constexpr operations
    constexpr aegis::ui::SizeMode const_mode = aegis::ui::SizeMode::Fixed;
    static_assert(const_mode == aegis::ui::SizeMode::Fixed,
                  "SizeMode must be constexpr accessible");
}

// Runtime tests for LayoutSpec
void test_layout_spec() {
    // Test construction with fixed sizes
    const aegis::ui::LayoutSpec spec1{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                      100.0f, 200.0f};
    const aegis::ui::LayoutSpec spec2{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                      100.0f, 200.0f};
    const aegis::ui::LayoutSpec spec3{aegis::ui::SizeMode::Flex, aegis::ui::SizeMode::Flex, 1.0f,
                                      2.0f};

    // Test equality
    assert(spec1 == spec2);
    assert(!(spec1 != spec2));

    // Test inequality
    assert(spec1 != spec3);
    assert(!(spec1 == spec3));

    // Test value access
    assert(spec1.width_mode == aegis::ui::SizeMode::Fixed);
    assert(spec1.height_mode == aegis::ui::SizeMode::Fixed);
    assert(spec1.width_value == 100.0f);
    assert(spec1.height_value == 200.0f);

    assert(spec3.width_mode == aegis::ui::SizeMode::Flex);
    assert(spec3.height_mode == aegis::ui::SizeMode::Flex);
    assert(spec3.width_value == 1.0f);
    assert(spec3.height_value == 2.0f);

    // Test constexpr operations
    constexpr aegis::ui::LayoutSpec const_spec{aegis::ui::SizeMode::Fixed,
                                               aegis::ui::SizeMode::Flex, 50.0f, 1.5f};
    static_assert(const_spec.width_mode == aegis::ui::SizeMode::Fixed,
                  "LayoutSpec width_mode must be constexpr accessible");
    static_assert(const_spec.height_mode == aegis::ui::SizeMode::Flex,
                  "LayoutSpec height_mode must be constexpr accessible");
    static_assert(const_spec.width_value == 50.0f,
                  "LayoutSpec width_value must be constexpr accessible");
    static_assert(const_spec.height_value == 1.5f,
                  "LayoutSpec height_value must be constexpr accessible");
}

// Test that LayoutIntent is an alias for LayoutSpec
void test_layout_intent() {
    // LayoutIntent should be the same as LayoutSpec
    const aegis::ui::LayoutIntent intent{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Fixed,
                                         100.0f, 100.0f};

    // Test it has the same structure and can be used interchangeably
    assert(intent.width_mode == aegis::ui::SizeMode::Fixed);
    assert(intent.height_mode == aegis::ui::SizeMode::Fixed);
    assert(intent.width_value == 100.0f);
    assert(intent.height_value == 100.0f);

    // Test constexpr construction
    constexpr aegis::ui::LayoutIntent const_intent{aegis::ui::SizeMode::Flex,
                                                   aegis::ui::SizeMode::Fixed, 2.0f, 75.0f};
    static_assert(const_intent.width_mode == aegis::ui::SizeMode::Flex,
                  "LayoutIntent must be constexpr constructible");
}

// Test mixed layout specifications
void test_mixed_layouts() {
    // Fixed width, flex height
    const aegis::ui::LayoutSpec mixed1{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Flex,
                                       200.0f, 1.0f};
    assert(mixed1.width_mode == aegis::ui::SizeMode::Fixed);
    assert(mixed1.height_mode == aegis::ui::SizeMode::Flex);
    assert(mixed1.width_value == 200.0f); // pixels
    assert(mixed1.height_value == 1.0f);  // weight

    // Flex width, fixed height
    const aegis::ui::LayoutSpec mixed2{aegis::ui::SizeMode::Flex, aegis::ui::SizeMode::Fixed, 3.0f,
                                       150.0f};
    assert(mixed2.width_mode == aegis::ui::SizeMode::Flex);
    assert(mixed2.height_mode == aegis::ui::SizeMode::Fixed);
    assert(mixed2.width_value == 3.0f);    // weight
    assert(mixed2.height_value == 150.0f); // pixels
}

// Test that there's no implicit computation logic
void test_no_computation_logic() {
    // Layout types should be pure data - no computation
    const aegis::ui::FixedSize fixed{100.0f};
    assert(fixed.pixels == 100.0f); // Direct value, no computation

    const aegis::ui::FlexSize flex{2.0f};
    assert(flex.weight == 2.0f); // Direct value, no computation

    const aegis::ui::LayoutSpec spec{aegis::ui::SizeMode::Fixed, aegis::ui::SizeMode::Flex, 50.0f,
                                     1.0f};
    // All values are stored as-is, no computation
    assert(spec.width_value == 50.0f);
    assert(spec.height_value == 1.0f);
}

int main() {
    test_fixed_size();
    test_flex_size();
    test_size_mode();
    test_layout_spec();
    test_layout_intent();
    test_mixed_layouts();
    test_no_computation_logic();
    return 0;
}
