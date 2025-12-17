# AegisUI Widget Lifecycle Specification

**Version:** 0.1.0  
**Status:** FROZEN  
**Last Updated:** 2025-12-17

This document formally specifies the lifecycle of widgets (nodes) in AegisUI. This is a **normative specification** — implementations must conform to these rules.

---

## 1. Widget Definition

In AegisUI, a **widget** is a node in the scene graph.

**Widget types:**
- `Node` — Basic container
- `TextNode` — Text leaf
- `GridNode` — Grid layout container

All widgets are **data structures**, not objects with behavior.

---

## 2. Lifecycle Stages

Every widget proceeds through exactly these stages:

```
Creation → Finalization → Consumption → Destruction
```

**No stages may be skipped.**  
**No stages may be reordered.**

---

## 3. Stage 1: Creation

### 3.1 Creation Method

Widgets are created by calling `SceneBuilder` methods:

```cpp
SceneBuilder builder(arena);

// Create Node
auto h1 = builder.add_node(id, layout, style_id, children);

// Create TextNode
auto h2 = builder.add_text_node(id, layout, style_id, content);

// Create GridNode
auto h3 = builder.add_grid_node(id, layout, style_id, columns, children);
```

### 3.2 Creation Semantics

During creation:

1. **Allocation occurs in arena**
   - Widget storage allocated via `arena.allocate()`
   - Guaranteed 8-byte alignment
   - No heap allocation

2. **Placement construction**
   - Widget constructed in-place via placement new
   - Members initialized from parameters
   - No virtual dispatch

3. **Handle assignment**
   - Sequential `NodeHandle` assigned
   - Handle value = creation order index
   - Handle returned to caller

4. **No evaluation**
   - No widget methods called
   - No traversal occurs
   - No layout computation

### 3.3 Creation Guarantees

**Determinism:**
- Same creation calls → same memory layout
- Same creation order → same handles

**Performance:**
- O(1) per widget
- No hidden allocations
- No virtual calls

**Safety:**
- No self-mutation possible (widgets are POD)
- No callbacks executed
- No user code invoked

---

## 4. Stage 2: Finalization

### 4.1 Finalization Method

Scene finalized via:
```cpp
Scene scene = builder.finalize(roots);
```

### 4.2 Finalization Semantics

During finalization:

1. **Builder marked finalized**
   - `finalized_` flag set to `true`
   - No more `add_*()` calls permitted

2. **Root handles stored**
   - Root span copied into arena
   - Scene struct constructed with root span

3. **Scene becomes immutable**
   - No further modifications allowed
   - All widgets frozen in current state

### 4.3 Widget State After Finalization

Widgets are:
- ✅ Immutable
- ✅ Readable
- ✅ Arena-allocated
- ✅ Referenceable via handles

Widgets are **not**:
- ❌ Mutable
- ❌ Removable
- ❌ Reorderable
- ❌ Separately destructible

### 4.4 Finalization Guarantees

**Immutability:**
- No widget fields may change
- No child lists may change
- No handle reassignment

**Validity:**
- All handles remain valid
- All spans remain valid
- Scene remains usable until arena reset

---

## 5. Stage 3: Consumption

### 5.1 Consumption Method

Scene consumed by Aegis Core (external to AegisUI):
```cpp
aegis_core_consume(scene);
```

### 5.2 Consumption Semantics

**AegisUI role:**
- Provide immutable scene
- Guarantee handle validity
- Maintain memory stability

**Aegis Core role:**
- Read scene structure
- Resolve layout
- Compute diff (if applicable)
- Route events

### 5.3 Widget State During Consumption

Widgets are:
- ✅ Readable by Aegis Core
- ✅ Immutable
- ✅ Valid (arena not reset)

Aegis Core may:
- ✅ Read widget fields
- ✅ Traverse via handles
- ✅ Compare across frames

Aegis Core may **not**:
- ❌ Mutate widgets
- ❌ Allocate new widgets
- ❌ Destroy widgets

### 5.4 Multi-Consumer Safety

Scene may be consumed by multiple systems:
- Layout engine
- Diff algorithm
- Event router
- Renderer (via Aegis Core)

**Concurrent reads are safe** (scene is immutable).

**Concurrent writes are forbidden** (scene is immutable).

---

## 6. Stage 4: Destruction

### 6.1 Destruction Method

Widgets destroyed via arena reset:
```cpp
arena.reset();
```

### 6.2 Destruction Semantics

During arena reset:

1. **All allocations freed**
   - Arena offset returns to 0
   - Widget memory reclaimed

2. **No destructors called**
   - Widgets are trivially destructible POD types
   - No cleanup code executes

3. **Handles invalidated**
   - All `NodeHandle` values become dangling
   - Scene becomes invalid

4. **Spans invalidated**
   - All `std::span` references become dangling
   - Accessing them is undefined behavior

### 6.3 Destruction Guarantees

**Performance:**
- O(1) destruction (single offset reset)
- No per-widget cleanup
- No virtual calls

**Safety:**
- No use-after-free possible (no post-destruction access)
- No memory leaks (arena owns all memory)
- No resource leaks (widgets have no resources)

---

## 7. Lifecycle Invariants

The following invariants hold throughout the lifecycle:

### 7.1 No Self-Mutation

**Invariant:** Widgets cannot modify themselves.

**Reasoning:**
- Widgets are POD structs
- No methods (except equality operators)
- No mutable state

**Enforcement:**
- `const` correctness in API
- Immutability after finalization

### 7.2 No Hidden Allocations

**Invariant:** Widget creation allocates exactly one widget in the arena.

**Reasoning:**
- All allocations explicit via `arena.allocate()`
- No heap allocation
- No STL container growth

**Enforcement:**
- Allocation guard tests
- Static analysis
- Code review

### 7.3 Stable Identity

**Invariant:** Widget identity (NodeId and NodeHandle) is stable for the frame.

**Reasoning:**
- `NodeId` provided by application, never changed
- `NodeHandle` assigned sequentially, never changed
- No widget removal or reordering

**Enforcement:**
- POD types (cannot be reassigned)
- Immutability after finalization

### 7.4 Frame-Local Lifetime

**Invariant:** Widgets live exactly one frame (creation → arena reset).

**Reasoning:**
- Arena-allocated
- No persistence across frames
- Destroyed on arena reset

**Enforcement:**
- Arena lifecycle model
- No cross-frame references

---

## 8. Lifecycle Contracts by Widget Type

### 8.1 Node

**Creation:**
```cpp
NodeHandle add_node(
    const NodeId& id,
    const LayoutSpec& layout,
    const StyleId& style_id,
    std::span<const NodeHandle> children
);
```

**Contract:**
- `children` span must remain valid for frame lifetime
- Child handles must be valid (previously created)
- Child handles may reference any widget type

**Finalization:** Standard (immutable)

**Consumption:** Children traversable via handles

**Destruction:** Standard (arena reset)

### 8.2 TextNode

**Creation:**
```cpp
NodeHandle add_text_node(
    const NodeId& id,
    const LayoutSpec& layout,
    const StyleId& style_id,
    std::string_view content
);
```

**Contract:**
- `content` must remain valid for frame lifetime
- Content typically arena-allocated (application responsibility)
- No implicit text copying

**Finalization:** Standard (immutable)

**Consumption:** Content readable, no measurement in AegisUI

**Destruction:** Standard (arena reset, content not freed by AegisUI)

### 8.3 GridNode

**Creation:**
```cpp
NodeHandle add_grid_node(
    const NodeId& id,
    const LayoutSpec& layout,
    const StyleId& style_id,
    std::uint32_t columns,
    std::span<const NodeHandle> children
);
```

**Contract:**
- `columns > 0` (not enforced, UB if violated)
- `children` span must remain valid for frame lifetime
- Child handles must be valid
- Row count = ceil(children.size() / columns)

**Finalization:** Standard (immutable)

**Consumption:** Children ordered row-major

**Destruction:** Standard (arena reset)

---

## 9. Lifecycle Anti-Patterns

The following are **forbidden lifecycle violations**:

### 9.1 Mutation After Finalization

❌ **Forbidden:**
```cpp
Scene scene = builder.finalize(roots);
// Cannot modify scene or widgets now
```

**Why:** Breaks immutability guarantee, violates contract.

### 9.2 Use After Arena Reset

❌ **Forbidden:**
```cpp
Scene scene = builder.finalize(roots);
arena.reset();
// Scene is now invalid, accessing it is UB
```

**Why:** Dangling references, undefined behavior.

### 9.3 Cross-Frame Handle Usage

❌ **Forbidden:**
```cpp
// Frame 1
NodeHandle h1 = builder1.add_node(...);

// Frame 2
NodeHandle h2 = builder2.add_node(...);
// Cannot compare h1 and h2, they're from different frames
```

**Why:** Handles are frame-local, comparison meaningless.

### 9.4 Manual Widget Destruction

❌ **Forbidden:**
```cpp
Node* node = /* get node pointer somehow */;
delete node; // Never do this
```

**Why:** Arena owns memory, manual `delete` is undefined behavior.

### 9.5 Widget Subclassing

❌ **Forbidden:**
```cpp
struct MyNode : Node {
    // Custom fields
};
```

**Why:** Widgets are POD, subclassing breaks size assumptions and contracts.

---

## 10. Lifecycle Performance Characteristics

### 10.1 Creation Cost

**Per-widget:**
- Time: O(1)
- Space: sizeof(widget)
- Allocations: 1 (in arena)

**No hidden costs:**
- No virtual dispatch
- No heap allocation
- No initialization beyond field assignment

### 10.2 Finalization Cost

**Total:**
- Time: O(r) where r = number of roots
- Space: sizeof(NodeHandle) * r
- Allocations: 1 (root span in arena)

### 10.3 Consumption Cost

**AegisUI cost:** O(1)

AegisUI does not participate in consumption. Cost determined by Aegis Core.

### 10.4 Destruction Cost

**Total:**
- Time: O(1) (arena offset reset)
- Space: 0 (memory reclaimed)
- Operations: 1 (offset = 0)

**No hidden costs:**
- No destructor calls
- No per-widget cleanup
- No virtual dispatch

---

## 11. Lifecycle Testing

### 11.1 Required Tests

Implementations must include tests for:

1. **Creation determinism**
   - Same input → same handles
   - Same input → same memory layout

2. **Finalization immutability**
   - Cannot add nodes after finalization
   - Cannot finalize twice

3. **Consumption safety**
   - Scene readable after finalization
   - Scene valid until arena reset

4. **Destruction completeness**
   - Arena reset invalidates scene
   - No memory leaks

### 11.2 Test Examples

See:
- `tests/scene_builder_test.cpp` — Creation and finalization
- `tests/determinism_test.cpp` — Lifecycle determinism
- `tests/arena_test.cpp` — Arena lifecycle
- `tests/allocation_guard_test.cpp` — No hidden allocations

---

## 12. Lifecycle Guarantees Summary

| Stage | Mutability | Validity | Cost |
|-------|------------|----------|------|
| Creation | Mutable (during creation) | Valid | O(1) |
| Finalization | Immutable | Valid | O(r) |
| Consumption | Immutable | Valid | O(1)* |
| Destruction | N/A | Invalid | O(1) |

\* AegisUI cost only; Aegis Core cost varies

---

## 13. Comparison with Traditional Widget Systems

| Property | AegisUI | Traditional (React/Flutter) |
|----------|---------|------------------------------|
| Lifecycle stages | 4 (fixed) | Variable (mount, update, unmount, etc.) |
| Self-mutation | None | Callbacks, setState() |
| Hidden allocations | None | Component state, closures |
| Destruction | Batch (arena reset) | Per-widget (GC or manual) |
| Identity stability | Frame-local | Persistent |
| Lifecycle callbacks | None | Many (componentDidMount, etc.) |

**Key difference:** AegisUI widgets are **data**, not **objects with behavior**.

---

## 14. Future Lifecycle Extensions

### 14.1 Not Planned

The following are **not planned** and would violate the AegisUI model:

❌ Widget update callbacks  
❌ Lifecycle hooks  
❌ Persistent widget state  
❌ Cross-frame widget identity  
❌ Incremental updates  

### 14.2 Potentially Compatible

The following **might** be compatible with future versions:

⚠️ Additional widget types (if POD and immutable)  
⚠️ Lifecycle observability (read-only, no mutation)  

Any such extensions require spec updates and major version bump.

---

## 15. Conformance

### 15.1 Implementation Requirements

A conforming implementation must:
1. Follow all 4 lifecycle stages in order
2. Guarantee no self-mutation
3. Guarantee no hidden allocations
4. Guarantee stable identity per frame

### 15.2 Application Requirements

A conforming application must:
1. Not access widgets after arena reset
2. Not mutate widgets after finalization
3. Not compare handles across frames
4. Provide valid spans/strings for widget lifetime

---

## 16. References

- **[SPEC_EXECUTION.md](SPEC_EXECUTION.md)** — Execution model and frame lifecycle
- **[SPEC_API.md](SPEC_API.md)** — Public API surface
- **[SPEC_INVARIANTS.md](SPEC_INVARIANTS.md)** — Determinism and memory guarantees
- **[HLD.md](HLD.md)** — High-level design
- **[LLD.md](LLD.md)** — Low-level design

---

**END OF SPECIFICATION**
