#pragma once

#include <string_view>

#include "ids.hpp"
#include "layout.hpp"
#include "style.hpp"

namespace aegis::ui {

/// Text leaf node in the scene graph
/// Immutable after construction
/// Contains text content reference with no ownership
///
/// **Content Lifetime Rules:**
/// - TextNode stores a non-owning reference (string_view) to text content
/// - Content must remain valid for the lifetime of the Scene
/// - Content is typically allocated in the same arena as the Scene
/// - No allocations occur during TextNode construction
/// - No implicit text measurement or layout computation
struct TextNode {
    NodeId id;
    LayoutSpec layout;
    StyleId style_id;
    std::string_view content; // Non-owning reference to text content
};

} // namespace aegis::ui
