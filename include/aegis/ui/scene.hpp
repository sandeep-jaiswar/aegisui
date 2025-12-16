#pragma once

#include <span>

#include "ids.hpp"

namespace aegis::ui {

/// Immutable scene graph
/// Built once per frame, no mutation allowed
/// Contains only NodeHandles - no direct node references
struct Scene {
    std::span<const NodeHandle> roots;
};

} // namespace aegis::ui
