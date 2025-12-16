#pragma once

#include "node.hpp"
#include "scene.hpp"

namespace aegis::ui {

/// Single-use scene builder
/// Allocates nodes in caller-provided arena
class SceneBuilder {
  public:
    SceneBuilder() = default;
    ~SceneBuilder() = default;

    // Non-copyable, non-movable
    SceneBuilder(const SceneBuilder&) = delete;
    SceneBuilder& operator=(const SceneBuilder&) = delete;
    SceneBuilder(SceneBuilder&&) = delete;
    SceneBuilder& operator=(SceneBuilder&&) = delete;

    /// Add a node to the scene
    /// Returns handle for referencing as child
    /// NOTE: This is a minimal stub implementation returning invalid handle
    NodeHandle add_node(const NodeId& id, const LayoutSpec& layout, const StyleId& style_id) noexcept {
        (void)id;
        (void)layout;
        (void)style_id;
        // Return invalid handle - full implementation will use arena allocation
        return NodeHandle{0};
    }

    /// Finalize the scene (makes it immutable)
    Scene finalize() noexcept { return Scene{}; }
};

} // namespace aegis::ui
