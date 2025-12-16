#pragma once

#include <cstdint>

namespace aegis::ui {

/// Stable, application-provided identifier for UI nodes
/// Used for diffing and event targeting across frames
struct NodeId {
    std::uint64_t value;

    constexpr bool operator==(const NodeId& other) const noexcept { return value == other.value; }

    constexpr bool operator!=(const NodeId& other) const noexcept { return value != other.value; }
};

/// Internal, frame-local index into arena storage
/// Never exposed outside Scene
struct NodeHandle {
    std::uint32_t value;

    constexpr bool operator==(const NodeHandle& other) const noexcept {
        return value == other.value;
    }

    constexpr bool operator!=(const NodeHandle& other) const noexcept {
        return value != other.value;
    }
};

} // namespace aegis::ui
