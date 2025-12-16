#pragma once

#include <cstdint>
#include <span>

#include "ids.hpp"

namespace aegis::ui {

/// RGBA color value
/// Compile-time constructible, POD type
struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;

    constexpr bool operator==(const Color& other) const noexcept {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    constexpr bool operator!=(const Color& other) const noexcept { return !(*this == other); }
};

/// Padding specification - spacing inside an element
/// Compile-time constructible, POD type
struct Padding {
    float top;
    float right;
    float bottom;
    float left;

    constexpr bool operator==(const Padding& other) const noexcept {
        return top == other.top && right == other.right && bottom == other.bottom &&
               left == other.left;
    }

    constexpr bool operator!=(const Padding& other) const noexcept { return !(*this == other); }
};

/// Border specification - edge styling
/// Compile-time constructible, POD type
struct Border {
    Color color;
    float width;

    constexpr bool operator==(const Border& other) const noexcept {
        return color == other.color && width == other.width;
    }

    constexpr bool operator!=(const Border& other) const noexcept { return !(*this == other); }
};

/// Fully resolved style data
/// No inheritance, no cascading, no runtime computation
/// Compile-time constructible, POD type with trivial destructor
struct Style {
    Color background_color;
    Border border;
    Padding padding;

    constexpr bool operator==(const Style& other) const noexcept {
        return background_color == other.background_color && border == other.border &&
               padding == other.padding;
    }

    constexpr bool operator!=(const Style& other) const noexcept { return !(*this == other); }
};

/// Style lookup entry - maps StyleId to resolved Style
/// Constant-time lookup, no cascading or inheritance
struct StyleEntry {
    StyleId id;
    Style style;

    constexpr bool operator==(const StyleEntry& other) const noexcept {
        return id == other.id && style == other.style;
    }

    constexpr bool operator!=(const StyleEntry& other) const noexcept { return !(*this == other); }
};

/// StyleSheet - immutable collection of fully resolved styles
/// Provides efficient style lookup via linear search over small sets
/// No cascading, no inheritance, no runtime computation
struct StyleSheet {
    std::span<const StyleEntry> entries;

    /// Lookup style by ID
    /// Returns pointer to Style if found, nullptr otherwise
    /// Linear search O(n) - efficient for small style sets (expected usage pattern)
    constexpr const Style* lookup(const StyleId& id) const noexcept {
        for (const auto& entry : entries) {
            if (entry.id == id) {
                return &entry.style;
            }
        }
        return nullptr;
    }
};

} // namespace aegis::ui
