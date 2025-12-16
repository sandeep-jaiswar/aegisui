#pragma once

#include "scene.hpp"
#include "node.hpp"

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
    NodeHandle add_node(const NodeId& id, const LayoutIntent& layout, const Style& style) noexcept {
        (void)id;
        (void)layout;
        (void)style;
        return NodeHandle{0};
    }

    /// Finalize the scene (makes it immutable)
    Scene finalize() noexcept {
        return Scene{};
    }
};

} // namespace aegis::ui
