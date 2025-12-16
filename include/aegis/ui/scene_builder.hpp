#pragma once

#include <cassert>
#include <cstdint>

#include "arena.hpp"
#include "grid_node.hpp"
#include "node.hpp"
#include "scene.hpp"
#include "text_node.hpp"

namespace aegis::ui {

/// Single-use scene builder
/// Allocates nodes in caller-provided arena
/// Zero heap allocations - all data stored in arena
/// Builder is single-use - cannot be reused after finalize()
class SceneBuilder {
  public:
    /// Construct builder with arena for allocations
    explicit SceneBuilder(Arena& arena) noexcept : arena_(arena), node_count_(0), finalized_(false) {}

    ~SceneBuilder() = default;

    // Non-copyable, non-movable
    SceneBuilder(const SceneBuilder&) = delete;
    SceneBuilder& operator=(const SceneBuilder&) = delete;
    SceneBuilder(SceneBuilder&&) = delete;
    SceneBuilder& operator=(SceneBuilder&&) = delete;

    /// Add a regular node to the scene
    /// Returns handle for referencing as child
    /// Children must be specified as span - lifetime managed by caller
    [[nodiscard]] NodeHandle add_node(const NodeId& id, const LayoutSpec& layout,
                                       const StyleId& style_id,
                                       std::span<const NodeHandle> children) noexcept {
        assert(!finalized_ && "Cannot add nodes after finalization");

        // Allocate Node in arena
        void* mem = arena_.allocate(sizeof(Node));
        if (mem == nullptr) {
            return NodeHandle{0}; // Out of memory
        }

        // Construct Node in-place
        Node* node = new (mem) Node{id, layout, style_id, children};
        (void)node; // Node is in arena, we don't need to track pointer

        // Return handle (index in registration order)
        return NodeHandle{node_count_++};
    }

    /// Add a text node to the scene
    /// Content must remain valid for scene lifetime (typically arena-allocated)
    [[nodiscard]] NodeHandle add_text_node(const NodeId& id, const LayoutSpec& layout,
                                            const StyleId& style_id,
                                            std::string_view content) noexcept {
        assert(!finalized_ && "Cannot add nodes after finalization");

        // Allocate TextNode in arena
        void* mem = arena_.allocate(sizeof(TextNode));
        if (mem == nullptr) {
            return NodeHandle{0}; // Out of memory
        }

        // Construct TextNode in-place
        TextNode* node = new (mem) TextNode{id, layout, style_id, content};
        (void)node; // Node is in arena, we don't need to track pointer

        // Return handle (index in registration order)
        return NodeHandle{node_count_++};
    }

    /// Add a grid node to the scene
    /// Children must be specified as span - lifetime managed by caller
    [[nodiscard]] NodeHandle add_grid_node(const NodeId& id, const LayoutSpec& layout,
                                            const StyleId& style_id, std::uint32_t columns,
                                            std::span<const NodeHandle> children) noexcept {
        assert(!finalized_ && "Cannot add nodes after finalization");

        // Allocate GridNode in arena
        void* mem = arena_.allocate(sizeof(GridNode));
        if (mem == nullptr) {
            return NodeHandle{0}; // Out of memory
        }

        // Construct GridNode in-place
        GridNode* node = new (mem) GridNode{id, layout, style_id, columns, children};
        (void)node; // Node is in arena, we don't need to track pointer

        // Return handle (index in registration order)
        return NodeHandle{node_count_++};
    }

    /// Finalize the scene (makes it immutable)
    /// Builder is single-use - cannot add more nodes or finalize again
    /// Root handles must be provided - these are the top-level nodes in the scene
    [[nodiscard]] Scene finalize(std::span<const NodeHandle> roots) noexcept {
        assert(!finalized_ && "Cannot finalize twice");
        finalized_ = true;

        // Allocate space for root handles in arena
        if (roots.empty()) {
            return Scene{std::span<const NodeHandle>{}};
        }

        void* mem = arena_.allocate(roots.size() * sizeof(NodeHandle));
        if (mem == nullptr) {
            return Scene{std::span<const NodeHandle>{}}; // Out of memory
        }

        // Copy root handles to arena
        auto* arena_roots = static_cast<NodeHandle*>(mem);
        for (std::size_t i = 0; i < roots.size(); ++i) {
            arena_roots[i] = roots[i];
        }

        return Scene{std::span<const NodeHandle>{arena_roots, roots.size()}};
    }

  private:
    Arena& arena_;
    std::uint32_t node_count_; // Track number of registered nodes
    bool finalized_;
};

} // namespace aegis::ui
