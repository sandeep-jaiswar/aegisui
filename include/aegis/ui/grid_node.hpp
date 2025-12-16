#pragma once

#include <cstdint>
#include <span>

#include "ids.hpp"
#include "layout.hpp"
#include "style.hpp"

namespace aegis::ui {

/// Grid node in the scene graph with explicit column count
/// Immutable after construction
/// Children are ordered row-major (left-to-right, top-to-bottom)
/// No auto-placement logic - layout resolution happens in Aegis Core
///
/// **Grid Layout Rules:**
/// - Explicit column count specified at construction
/// - Children fill cells in row-major order
/// - Number of rows determined by children count and column count
/// - No implicit grid sizing or auto-placement
struct GridNode {
    NodeId id;
    LayoutSpec layout;
    StyleId style_id;
    std::uint32_t columns;                // Explicit column count for grid layout
    std::span<const NodeHandle> children; // Row-major ordered children
};

} // namespace aegis::ui
