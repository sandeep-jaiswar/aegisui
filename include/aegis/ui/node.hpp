#pragma once

#include <span>

#include "ids.hpp"
#include "layout.hpp"
#include "style.hpp"

namespace aegis::ui {

/// UI node in the scene graph
/// Immutable after construction
/// Contains only identifiers and layout specification - no virtual functions
struct Node {
    NodeId id;
    LayoutSpec layout;
    StyleId style_id;
    std::span<const NodeHandle> children;
};

} // namespace aegis::ui
