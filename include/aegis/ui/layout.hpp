#pragma once

#include <cstdint>

namespace aegis::ui {

/// Layout sizing mode - declarative intent only
/// Resolution happens in Aegis Core
enum class SizeMode : std::uint8_t {
    Fixed,  // Explicit size in pixels
    Flex    // Flexible sizing
};

/// Layout intent structure
/// No content-based sizing, percentages, or auto rules
struct LayoutIntent {
    SizeMode width_mode;
    SizeMode height_mode;
    float width_value;   // Used when width_mode is Fixed
    float height_value;  // Used when height_mode is Fixed
};

} // namespace aegis::ui
