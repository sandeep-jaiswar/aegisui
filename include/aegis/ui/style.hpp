#pragma once

#include <cstdint>

namespace aegis::ui {

/// RGBA color value
struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
};

/// Fully resolved style data
/// No inheritance, no cascading, no runtime computation
struct Style {
    Color background_color;
    Color border_color;
    float border_width;
    float padding_top;
    float padding_right;
    float padding_bottom;
    float padding_left;
};

} // namespace aegis::ui
