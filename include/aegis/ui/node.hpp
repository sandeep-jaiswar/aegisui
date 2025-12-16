#pragma once

#include <span>

#include "ids.hpp"
#include "layout.hpp"
#include "style.hpp"

namespace aegis::ui {

/// UI node in the scene graph
/// Immutable after construction
struct Node {
    NodeId id;
    LayoutIntent layout;
    Style style;
    std::span<const NodeHandle> children;
};

} // namespace aegis::ui
