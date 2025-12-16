#pragma once

#include "ids.hpp"
#include "layout.hpp"
#include "style.hpp"
#include <span>

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
