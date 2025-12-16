#pragma once

#include "node.hpp"
#include "ids.hpp"
#include <span>

namespace aegis::ui {

/// Immutable scene graph
/// Built once per frame, no mutation allowed
class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    // Non-copyable, non-movable
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    /// Get all nodes in the scene
    std::span<const Node> nodes() const noexcept {
        return {};
    }

    /// Get root node handles
    std::span<const NodeHandle> roots() const noexcept {
        return {};
    }
};

} // namespace aegis::ui
