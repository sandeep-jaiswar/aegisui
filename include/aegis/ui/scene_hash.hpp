#pragma once

#include <cstdint>

#include "scene.hpp"

namespace aegis::ui {

/// Compute deterministic hash of a scene
/// Hash is independent of memory addresses and depends only on logical structure
/// Same scene structure always produces the same hash value
/// 
/// The hash is computed using FNV-1a algorithm for simplicity and efficiency:
/// - Deterministic: same inputs always produce same output
/// - Memory-address independent: uses only values, not pointers
/// - Fast: single pass over scene roots
[[nodiscard]] inline std::uint64_t hash_scene(const Scene& scene) noexcept {
    // FNV-1a constants for 64-bit hash
    constexpr std::uint64_t fnv_offset_basis = 14695981039346656037ULL;
    constexpr std::uint64_t fnv_prime = 1099511628211ULL;

    std::uint64_t hash = fnv_offset_basis;

    // Hash the number of roots first
    const std::uint64_t root_count = scene.roots.size();
    hash ^= root_count;
    hash *= fnv_prime;

    // Hash each root handle value
    for (const auto& root : scene.roots) {
        hash ^= root.value;
        hash *= fnv_prime;
    }

    return hash;
}

} // namespace aegis::ui
