#pragma once

#include <cstdint>

namespace aegis::ui {

/// Fixed size specification - explicit pixel dimensions
/// Compile-time constructible, POD type
struct FixedSize {
    float pixels;

    constexpr bool operator==(const FixedSize& other) const noexcept {
        return pixels == other.pixels;
    }

    constexpr bool operator!=(const FixedSize& other) const noexcept {
        return pixels != other.pixels;
    }
};

/// Flex size specification - flexible sizing with weight
/// Compile-time constructible, POD type
struct FlexSize {
    float weight;

    constexpr bool operator==(const FlexSize& other) const noexcept {
        return weight == other.weight;
    }

    constexpr bool operator!=(const FlexSize& other) const noexcept {
        return weight != other.weight;
    }
};

/// Layout sizing mode - declarative intent only
/// Resolution happens in Aegis Core
enum class SizeMode : std::uint8_t {
    Fixed, // Explicit size in pixels
    Flex   // Flexible sizing
};

/// Layout specification combining width and height sizing
/// Compile-time constructible, POD type with trivial destructor
struct LayoutSpec {
    SizeMode width_mode;
    SizeMode height_mode;
    float width_value;  // Used when width_mode is Fixed (pixels) or Flex (weight)
    float height_value; // Used when height_mode is Fixed (pixels) or Flex (weight)

    constexpr bool operator==(const LayoutSpec& other) const noexcept {
        return width_mode == other.width_mode && height_mode == other.height_mode &&
               width_value == other.width_value && height_value == other.height_value;
    }

    constexpr bool operator!=(const LayoutSpec& other) const noexcept { return !(*this == other); }
};

/// Layout intent structure - declarative layout description
/// No content-based sizing, percentages, or auto rules
/// Alias for LayoutSpec to maintain API compatibility
using LayoutIntent = LayoutSpec;

} // namespace aegis::ui
