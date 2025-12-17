# AegisUI Public API Specification

**Version:** 0.1.0  
**Status:** FROZEN  
**Last Updated:** 2025-12-17

This document defines the **complete public API surface** of AegisUI. This is a **normative specification** — only interfaces listed here are public and stable.

**Rule:** Anything not explicitly listed in this document is **private, unstable, and subject to change without notice**.

---

## 1. Public API Stability Contract

### 1.1 Frozen Surface

As of version 0.1.0, the public API is **FROZEN**.

**No additions are permitted** until a formal unfreezing process occurs.

### 1.2 What "Public" Means

A public API element:
- Is explicitly listed in this document
- Is documented
- Is stable across patch versions
- May only change in major version bumps

### 1.3 What "Private" Means

Elements not listed here:
- May change without notice
- May be removed
- May have breaking changes in patch versions
- Should not be used by application code

---

## 2. Public Headers

The following headers constitute the **complete public interface**:

### 2.1 Core Headers (Required)

| Header | Purpose | Status |
|--------|---------|--------|
| `aegis/ui/ids.hpp` | Identity types (`NodeId`, `StyleId`, `NodeHandle`) | FROZEN |
| `aegis/ui/layout.hpp` | Layout intent structures | FROZEN |
| `aegis/ui/style.hpp` | Style data structures | FROZEN |
| `aegis/ui/node.hpp` | Basic node type | FROZEN |
| `aegis/ui/text_node.hpp` | Text node type | FROZEN |
| `aegis/ui/grid_node.hpp` | Grid node type | FROZEN |
| `aegis/ui/scene.hpp` | Scene container | FROZEN |
| `aegis/ui/scene_builder.hpp` | Scene construction API | FROZEN |
| `aegis/ui/arena.hpp` | Arena allocator | FROZEN |

### 2.2 Utility Headers (Optional)

| Header | Purpose | Status |
|--------|---------|--------|
| `aegis/ui/scene_hash.hpp` | Scene hashing for determinism testing | FROZEN |

### 2.3 Header Inclusion Order

**No specific inclusion order is required.**

All public headers are self-contained and include necessary dependencies.

### 2.4 Namespace

All public APIs are in namespace:
```cpp
namespace aegis::ui
```

**No other namespaces are public.**

In particular:
- `aegis::ui::internal` (if it exists) is **private**
- `aegis::ui::detail` (if it exists) is **private**
- Nested anonymous namespaces are **private**

---

## 3. Identity Types (ids.hpp)

### 3.1 NodeId

```cpp
struct NodeId {
    std::uint64_t value;
    
    constexpr bool operator==(const NodeId& other) const noexcept;
    constexpr bool operator!=(const NodeId& other) const noexcept;
};
```

**Purpose:** Stable, application-provided identifier for UI nodes.

**Semantics:**
- Application controls `value` assignment
- Must be stable across frames for diffing
- Used by Aegis Core for event targeting
- Equality based on `value` comparison

**Constraints:**
- `value` should be unique per node in a scene (not enforced)
- Zero is a valid value

### 3.2 StyleId

```cpp
struct StyleId {
    std::uint64_t value;
    
    constexpr bool operator==(const StyleId& other) const noexcept;
    constexpr bool operator!=(const StyleId& other) const noexcept;
};
```

**Purpose:** Stable, application-provided identifier for styles.

**Semantics:**
- Application controls `value` assignment
- Used for style lookups in `StyleSheet`
- Equality based on `value` comparison

**Constraints:**
- `value` should map to a style in the active `StyleSheet` (not enforced)

### 3.3 NodeHandle

```cpp
struct NodeHandle {
    std::uint32_t value;
    
    constexpr bool operator==(const NodeHandle& other) const noexcept;
    constexpr bool operator!=(const NodeHandle& other) const noexcept;
};
```

**Purpose:** Internal, frame-local index into scene storage.

**Semantics:**
- Returned by `SceneBuilder::add_*()` methods
- Sequential starting from 0
- Valid only within the frame it was created
- Used to specify parent-child relationships

**Constraints:**
- Do not persist handles across frames
- Do not compare handles from different frames
- Treat as opaque within application code

---

## 4. Layout Types (layout.hpp)

### 4.1 SizeMode

```cpp
enum class SizeMode : std::uint8_t {
    Fixed,  // Explicit size in pixels
    Flex    // Flexible sizing with weight
};
```

**Purpose:** Discriminator for layout sizing mode.

**Values:**
- `Fixed`: Use explicit pixel size
- `Flex`: Use flexible sizing with weight factor

### 4.2 FixedSize

```cpp
struct FixedSize {
    float pixels;
    
    constexpr bool operator==(const FixedSize& other) const noexcept;
    constexpr bool operator!=(const FixedSize& other) const noexcept;
};
```

**Purpose:** Explicit pixel dimension.

**Semantics:**
- `pixels`: Dimension in pixels (positive)
- POD type, trivially copyable

### 4.3 FlexSize

```cpp
struct FlexSize {
    float weight;
    
    constexpr bool operator==(const FlexSize& other) const noexcept;
    constexpr bool operator!=(const FlexSize& other) const noexcept;
};
```

**Purpose:** Flexible sizing with weight factor.

**Semantics:**
- `weight`: Relative weight (positive)
- Layout resolved by Aegis Core
- POD type, trivially copyable

### 4.4 LayoutSpec

```cpp
struct LayoutSpec {
    SizeMode width_mode;
    SizeMode height_mode;
    float width_value;   // pixels if Fixed, weight if Flex
    float height_value;  // pixels if Fixed, weight if Flex
    
    constexpr bool operator==(const LayoutSpec& other) const noexcept;
    constexpr bool operator!=(const LayoutSpec& other) const noexcept;
};
```

**Purpose:** Complete layout specification for a node.

**Semantics:**
- Declarative intent only
- Resolution happens in Aegis Core
- POD type, trivially copyable

**Constraints:**
- `width_value` and `height_value` should be positive
- Interpretation depends on corresponding `*_mode`

### 4.5 LayoutIntent (Alias)

```cpp
using LayoutIntent = LayoutSpec;
```

**Purpose:** Semantic alias for `LayoutSpec`.

**Usage:** Identical to `LayoutSpec`.

---

## 5. Style Types (style.hpp)

### 5.1 Color

```cpp
struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
    
    constexpr bool operator==(const Color& other) const noexcept;
    constexpr bool operator!=(const Color& other) const noexcept;
};
```

**Purpose:** RGBA color value.

**Semantics:**
- `r`, `g`, `b`, `a`: Color channels (0-255)
- POD type, trivially copyable

### 5.2 Padding

```cpp
struct Padding {
    float top;
    float right;
    float bottom;
    float left;
    
    constexpr bool operator==(const Padding& other) const noexcept;
    constexpr bool operator!=(const Padding& other) const noexcept;
};
```

**Purpose:** Spacing inside an element.

**Semantics:**
- Edge offsets in pixels
- POD type, trivially copyable

**Constraints:**
- Values should be non-negative

### 5.3 Border

```cpp
struct Border {
    Color color;
    float width;
    
    constexpr bool operator==(const Border& other) const noexcept;
    constexpr bool operator!=(const Border& other) const noexcept;
};
```

**Purpose:** Border styling.

**Semantics:**
- `color`: Border color
- `width`: Border width in pixels
- POD type, trivially copyable

**Constraints:**
- `width` should be non-negative

### 5.4 Style

```cpp
struct Style {
    Color background_color;
    Border border;
    Padding padding;
    
    constexpr bool operator==(const Style& other) const noexcept;
    constexpr bool operator!=(const Style& other) const noexcept;
};
```

**Purpose:** Fully resolved style data.

**Semantics:**
- No inheritance
- No cascading
- No runtime computation
- POD type, trivially copyable

### 5.5 StyleEntry

```cpp
struct StyleEntry {
    StyleId id;
    Style style;
    
    constexpr bool operator==(const StyleEntry& other) const noexcept;
    constexpr bool operator!=(const StyleEntry& other) const noexcept;
};
```

**Purpose:** Maps `StyleId` to resolved `Style`.

**Semantics:**
- Used in `StyleSheet`
- POD type, trivially copyable

### 5.6 StyleSheet

```cpp
struct StyleSheet {
    std::span<const StyleEntry> entries;
    
    constexpr const Style* lookup(const StyleId& id) const noexcept;
};
```

**Purpose:** Immutable collection of fully resolved styles.

**Methods:**

#### lookup()
```cpp
constexpr const Style* lookup(const StyleId& id) const noexcept;
```

**Parameters:**
- `id`: StyleId to look up

**Returns:**
- Pointer to `Style` if found
- `nullptr` if not found

**Complexity:** O(n) linear search (efficient for small style sets)

---

## 6. Node Types

### 6.1 Node (node.hpp)

```cpp
struct Node {
    NodeId id;
    LayoutSpec layout;
    StyleId style_id;
    std::span<const NodeHandle> children;
};
```

**Purpose:** Basic container node in scene graph.

**Semantics:**
- Immutable after construction
- Can have zero or more children
- No ownership (arena-allocated)
- POD type, trivially copyable

**Constraints:**
- `children` must reference valid `NodeHandle` values
- `children` span must remain valid for scene lifetime

### 6.2 TextNode (text_node.hpp)

```cpp
struct TextNode {
    NodeId id;
    LayoutSpec layout;
    StyleId style_id;
    std::string_view content;
};
```

**Purpose:** Text leaf node.

**Semantics:**
- Immutable after construction
- No children (leaf node)
- `content`: Non-owning reference to text
- POD type, trivially copyable

**Constraints:**
- `content` must remain valid for scene lifetime
- Typically allocated in same arena as scene

### 6.3 GridNode (grid_node.hpp)

```cpp
struct GridNode {
    NodeId id;
    LayoutSpec layout;
    StyleId style_id;
    std::uint32_t columns;
    std::span<const NodeHandle> children;
};
```

**Purpose:** Grid layout container node.

**Semantics:**
- Immutable after construction
- Children ordered row-major
- Explicit column count
- POD type, trivially copyable

**Constraints:**
- `columns` should be > 0
- `children` must reference valid `NodeHandle` values
- Number of rows = ceil(children.size() / columns)

---

## 7. Scene (scene.hpp)

```cpp
struct Scene {
    std::span<const NodeHandle> roots;
};
```

**Purpose:** Immutable scene graph container.

**Semantics:**
- Built once per frame
- No mutation allowed after construction
- Contains only root node handles
- POD type, trivially copyable

**Constraints:**
- `roots` must reference valid `NodeHandle` values
- `roots` span must remain valid (arena-allocated)
- Scene invalid after arena reset

---

## 8. SceneBuilder (scene_builder.hpp)

```cpp
class SceneBuilder {
  public:
    explicit SceneBuilder(Arena& arena) noexcept;
    
    ~SceneBuilder() = default;
    
    // Non-copyable, non-movable
    SceneBuilder(const SceneBuilder&) = delete;
    SceneBuilder& operator=(const SceneBuilder&) = delete;
    SceneBuilder(SceneBuilder&&) = delete;
    SceneBuilder& operator=(SceneBuilder&&) = delete;
    
    [[nodiscard]] NodeHandle add_node(
        const NodeId& id,
        const LayoutSpec& layout,
        const StyleId& style_id,
        std::span<const NodeHandle> children
    ) noexcept;
    
    [[nodiscard]] NodeHandle add_text_node(
        const NodeId& id,
        const LayoutSpec& layout,
        const StyleId& style_id,
        std::string_view content
    ) noexcept;
    
    [[nodiscard]] NodeHandle add_grid_node(
        const NodeId& id,
        const LayoutSpec& layout,
        const StyleId& style_id,
        std::uint32_t columns,
        std::span<const NodeHandle> children
    ) noexcept;
    
    [[nodiscard]] Scene finalize(std::span<const NodeHandle> roots) noexcept;
};
```

**Purpose:** Scene construction API.

**Lifecycle:**
1. Construct with arena reference
2. Call `add_*()` methods to build scene
3. Call `finalize()` to complete scene
4. Builder is single-use, cannot be reused

### 8.1 Constructor

```cpp
explicit SceneBuilder(Arena& arena) noexcept;
```

**Parameters:**
- `arena`: Arena for allocations (must outlive builder)

**Effects:**
- Initializes builder state
- No allocations occur

### 8.2 add_node()

```cpp
[[nodiscard]] NodeHandle add_node(
    const NodeId& id,
    const LayoutSpec& layout,
    const StyleId& style_id,
    std::span<const NodeHandle> children
) noexcept;
```

**Parameters:**
- `id`: Application-provided node identifier
- `layout`: Layout specification
- `style_id`: Style identifier
- `children`: Child node handles (may be empty)

**Returns:** Handle to newly created node

**Effects:**
- Allocates `Node` in arena
- Assigns sequential `NodeHandle`
- Does not copy children span (stores reference)

**Preconditions:**
- Builder not finalized
- Arena has sufficient space
- Children handles are valid

### 8.3 add_text_node()

```cpp
[[nodiscard]] NodeHandle add_text_node(
    const NodeId& id,
    const LayoutSpec& layout,
    const StyleId& style_id,
    std::string_view content
) noexcept;
```

**Parameters:**
- `id`: Application-provided node identifier
- `layout`: Layout specification
- `style_id`: Style identifier
- `content`: Text content (non-owning reference)

**Returns:** Handle to newly created text node

**Effects:**
- Allocates `TextNode` in arena
- Assigns sequential `NodeHandle`
- Does not copy content (stores reference)

**Preconditions:**
- Builder not finalized
- Arena has sufficient space
- Content remains valid for scene lifetime

### 8.4 add_grid_node()

```cpp
[[nodiscard]] NodeHandle add_grid_node(
    const NodeId& id,
    const LayoutSpec& layout,
    const StyleId& style_id,
    std::uint32_t columns,
    std::span<const NodeHandle> children
) noexcept;
```

**Parameters:**
- `id`: Application-provided node identifier
- `layout`: Layout specification
- `style_id`: Style identifier
- `columns`: Number of columns in grid
- `children`: Child node handles in row-major order

**Returns:** Handle to newly created grid node

**Effects:**
- Allocates `GridNode` in arena
- Assigns sequential `NodeHandle`
- Does not copy children span (stores reference)

**Preconditions:**
- Builder not finalized
- Arena has sufficient space
- Children handles are valid
- `columns > 0`

### 8.5 finalize()

```cpp
[[nodiscard]] Scene finalize(std::span<const NodeHandle> roots) noexcept;
```

**Parameters:**
- `roots`: Root node handles (may be empty)

**Returns:** Immutable `Scene`

**Effects:**
- Marks builder as finalized
- Allocates root handle storage in arena
- Copies root handles to arena

**Preconditions:**
- Builder not already finalized
- Arena has sufficient space
- Root handles are valid

**Postconditions:**
- Builder cannot be used again
- Scene is immutable

---

## 9. Arena (arena.hpp)

```cpp
class Arena {
  public:
    constexpr Arena(std::span<std::byte> buffer) noexcept;
    
    [[nodiscard]] void* allocate(std::size_t size) noexcept;
    constexpr void reset() noexcept;
    
    [[nodiscard]] constexpr std::size_t offset() const noexcept;
    [[nodiscard]] constexpr std::size_t capacity() const noexcept;
    [[nodiscard]] constexpr std::size_t remaining() const noexcept;
};
```

**Purpose:** Frame-local arena allocator.

### 9.1 Constructor

```cpp
constexpr Arena(std::span<std::byte> buffer) noexcept;
```

**Parameters:**
- `buffer`: Pre-allocated buffer for arena (must outlive arena)

**Effects:**
- Initializes arena with offset 0

### 9.2 allocate()

```cpp
[[nodiscard]] void* allocate(std::size_t size) noexcept;
```

**Parameters:**
- `size`: Bytes to allocate

**Returns:**
- Pointer to allocated memory (8-byte aligned)
- `nullptr` if insufficient space

**Effects:**
- Advances internal offset
- No initialization of returned memory

### 9.3 reset()

```cpp
constexpr void reset() noexcept;
```

**Effects:**
- Resets offset to 0
- All previous allocations invalidated
- Does not call destructors

### 9.4 offset()

```cpp
[[nodiscard]] constexpr std::size_t offset() const noexcept;
```

**Returns:** Current offset (bytes used)

### 9.5 capacity()

```cpp
[[nodiscard]] constexpr std::size_t capacity() const noexcept;
```

**Returns:** Total buffer capacity

### 9.6 remaining()

```cpp
[[nodiscard]] constexpr std::size_t remaining() const noexcept;
```

**Returns:** Remaining space in buffer

---

## 10. Scene Hashing (scene_hash.hpp)

```cpp
[[nodiscard]] inline std::uint64_t hash_scene(const Scene& scene) noexcept;
```

**Purpose:** Compute deterministic hash of scene.

**Parameters:**
- `scene`: Scene to hash

**Returns:** 64-bit hash value

**Semantics:**
- Deterministic (same scene → same hash)
- Memory-address independent
- Based on FNV-1a algorithm
- Fast single-pass computation

**Usage:** Primarily for determinism testing and verification.

---

## 11. Forbidden APIs

The following are **explicitly NOT part of the public API**:

### 11.1 Forbidden Patterns

❌ Direct node allocation (use `SceneBuilder`)  
❌ Scene mutation after finalization  
❌ Manual memory management (`new`, `delete`)  
❌ Inheritance from node types  
❌ Subclassing `SceneBuilder` or `Arena`  
❌ RTTI (`dynamic_cast`, `typeid`)  
❌ Exceptions  

### 11.2 Forbidden Namespaces

❌ `aegis::ui::internal`  
❌ `aegis::ui::detail`  
❌ Any namespace not explicitly `aegis::ui`  

---

## 12. Versioning

### 12.1 API Version

This specification describes **AegisUI v0.1.0**.

### 12.2 Compatibility

**Patch versions (0.1.x):** No API changes  
**Minor versions (0.x.0):** Not applicable (API frozen)  
**Major versions (x.0.0):** May break API

### 12.3 Deprecation Policy

Since API is frozen, **no deprecations** until major version bump.

---

## 13. Enforcement

### 13.1 CI Checks

The following CI checks enforce this specification:
- ✅ No new public headers without approval
- ✅ No new public symbols without approval
- ✅ All public symbols documented

### 13.2 Namespace Enforcement

Use namespace visibility:
```cpp
namespace aegis::ui::internal {
    // Private, unstable APIs
}
```

Applications should not access `internal::` namespace.

---

## 14. References

- **[SPEC_EXECUTION.md](SPEC_EXECUTION.md)** — Execution model
- **[SPEC_WIDGET_LIFECYCLE.md](SPEC_WIDGET_LIFECYCLE.md)** — Widget lifecycle
- **[SPEC_INVARIANTS.md](SPEC_INVARIANTS.md)** — Determinism and memory guarantees
- **[HLD.md](HLD.md)** — High-level design
- **[LLD.md](LLD.md)** — Low-level design

---

**END OF SPECIFICATION**
