# AegisUI Execution Model Specification

**Version:** 0.1.0  
**Status:** FROZEN  
**Last Updated:** 2025-12-17

This document formally specifies the execution model of AegisUI. This is a **normative specification** — implementations must conform to these rules.

---

## 1. Execution Context

AegisUI operates as a **pure, synchronous function** that transforms application state into an immutable scene graph.

### 1.1 No Runtime

AegisUI has **no runtime**, **no event loop**, and **no background execution**.

Every scene construction is:
- Initiated explicitly by application code
- Executed synchronously on the calling thread
- Completed before control returns to the caller

### 1.2 Frame Boundaries

A **frame** is defined as:
1. Arena allocation
2. Scene construction via `SceneBuilder`
3. Scene finalization
4. Scene consumption by Aegis Core
5. Arena reset

Frames are **discrete and non-overlapping**. No state persists between frames except application-managed data.

---

## 2. Frame Lifecycle Stages

Each frame proceeds through exactly these stages, in order:

### Stage 1: Arena Preparation
```
Arena arena(buffer);
```
- Arena is constructed with caller-provided buffer
- Arena offset is 0
- No allocations have occurred
- Buffer must remain valid for entire frame lifetime

### Stage 2: Builder Creation
```
SceneBuilder builder(arena);
```
- Builder is created with reference to arena
- Builder state is initialized (node_count = 0, finalized = false)
- No nodes exist yet

### Stage 3: Node Construction
```
builder.add_node(...);
builder.add_text_node(...);
builder.add_grid_node(...);
```
- Application code calls builder methods to construct nodes
- Each call allocates node in arena
- Each call returns a `NodeHandle` (sequential index)
- Order of calls determines node registration order
- **No evaluation or layout occurs**

### Stage 4: Scene Finalization
```
Scene scene = builder.finalize(roots);
```
- Builder is marked as finalized
- Root handles are copied into arena
- Scene is constructed with immutable root span
- **Builder cannot be used after this point**

### Stage 5: Scene Consumption
```
aegis_core_consume(scene);
```
- Scene is passed to Aegis Core for layout and diffing
- Scene remains immutable during consumption
- Scene references arena memory only

### Stage 6: Arena Reset
```
arena.reset();
```
- All arena allocations are freed
- Arena offset returns to 0
- Scene becomes invalid (dangling references)
- Next frame may begin

---

## 3. Widget Evaluation Order

AegisUI does **not** evaluate widgets. There is no widget tree traversal.

### 3.1 Construction Order

Nodes are constructed in the order application code calls builder methods:
```cpp
// Construction order: node1, then node2, then node3
auto h1 = builder.add_node(NodeId{1}, ...);
auto h2 = builder.add_node(NodeId{2}, ...);
auto h3 = builder.add_node(NodeId{3}, ...);
```

NodeHandles are assigned sequentially starting from 0:
- First node → `NodeHandle{0}`
- Second node → `NodeHandle{1}`
- Third node → `NodeHandle{2}`

This order is **deterministic** and **reproducible**.

### 3.2 No Tree Traversal

AegisUI performs **no traversal** of the constructed scene:
- No parent-to-child iteration
- No child-to-parent iteration
- No sibling traversal

Scene structure is opaque to AegisUI after finalization.

---

## 4. Layout and Diff Sequencing

Layout and diffing are **not responsibilities of AegisUI**.

### 4.1 Layout (Aegis Core Responsibility)

After scene finalization:
1. Aegis Core receives immutable scene
2. Aegis Core performs layout resolution
3. Layout uses only declarative `LayoutSpec` intent
4. Layout produces concrete pixel coordinates
5. **AegisUI is not involved**

Layout ordering and algorithm are specified in **Aegis Core documentation**, not here.

### 4.2 Diffing (Aegis Core Responsibility)

Aegis Core may compare scenes across frames:
1. Previous frame scene (if any)
2. Current frame scene
3. Diff algorithm identifies changes
4. **AegisUI is not involved**

Diff ordering and algorithm are specified in **Aegis Core documentation**, not here.

### 4.3 AegisUI's Role

AegisUI guarantees that:
- Given identical inputs, scenes are byte-identical
- `NodeId` values are stable and application-controlled
- Scene structure is immutable after finalization

These guarantees enable Aegis Core to diff scenes reliably.

---

## 5. Interaction with Aegis Core

### 5.1 Data Flow

```
Application State
       ↓
   AegisUI Scene Construction
       ↓
   Immutable Scene
       ↓
   Aegis Core Layout Engine
       ↓
   Concrete Layout Tree
       ↓
   Renderer
```

### 5.2 Interface Contract

**AegisUI provides:**
- `Scene` struct with immutable root handles
- Node types: `Node`, `TextNode`, `GridNode`
- Layout intent: `LayoutSpec`
- Style references: `StyleId`

**Aegis Core provides:**
- Scene consumption API (external to AegisUI)
- Layout resolution
- Diff computation
- Event routing (using `NodeId` for targeting)

### 5.3 No Direct Communication

AegisUI and Aegis Core **do not communicate during scene construction**.

Scene construction is a pure function:
```
Scene = f(AppState, Arena)
```

Aegis Core interaction happens **after** scene construction completes.

---

## 6. Determinism Guarantees

### 6.1 Byte-Identical Output

Given:
- Same application state
- Same builder method calls in same order
- Same arena (sufficient capacity)

Then:
- Resulting `Scene` is **byte-identical** across runs
- Node handles are identical
- Memory layout in arena is identical

### 6.2 Platform Independence

Scene construction is platform-independent:
- No floating-point rounding (layout uses only POD types)
- No platform-specific alignment beyond 8-byte minimum
- No OS or hardware dependencies
- No undefined behavior

### 6.3 Replay Equivalence

Given an event log of application state changes:
1. Replaying the log produces identical scenes
2. Arena allocations occur in identical order
3. Node construction proceeds identically

This enables:
- Deterministic testing
- Session replay
- Bug reproduction

---

## 7. Memory and Allocation Model

### 7.1 Arena-Only Allocation

**All** allocations occur in the caller-provided arena:
- Node allocations
- Root handle storage
- No heap allocations (`new`, `malloc`) during construction

### 7.2 Allocation Order

Allocations occur in the order of builder method calls:
1. First `add_*()` call → first allocation
2. Second `add_*()` call → second allocation
3. `finalize()` → root handle allocation

This order is **deterministic** and **reproducible**.

### 7.3 Alignment

All allocations are 8-byte aligned minimum.

### 7.4 Lifetime

All allocations live until arena reset:
- No per-allocation deallocation
- No destructors called (nodes are POD)
- Memory reclaimed only on `arena.reset()`

---

## 8. Thread Safety

### 8.1 Single-Threaded Construction

`SceneBuilder` is **not thread-safe**:
- One thread constructs scene per frame
- No concurrent `add_*()` calls
- No concurrent `finalize()` calls

### 8.2 Immutable Scene Sharing

After finalization, `Scene` is immutable:
- Multiple threads may read scene concurrently
- No synchronization required for reads
- Scene must not be mutated

### 8.3 Arena Safety

`Arena` is **not thread-safe**:
- No concurrent allocations
- No concurrent resets
- Single-threaded use only

---

## 9. Error Handling

### 9.1 Allocation Failure

If arena has insufficient space:
- `allocate()` returns `nullptr`
- Assertion fails in debug builds
- Undefined behavior in release builds

**Application responsibility:**
- Provide sufficiently large arena buffer
- Monitor arena usage if needed

### 9.2 API Misuse

Misuse is detected via assertions in debug builds:
- Adding nodes after finalization → assertion failure
- Finalizing twice → assertion failure
- Invalid NodeHandles → no validation (UB)

**Application responsibility:**
- Use builder correctly
- Do not finalize multiple times
- Do not use builder after finalization

---

## 10. Execution Performance Characteristics

### 10.1 Time Complexity

- `add_node()`: O(1)
- `add_text_node()`: O(1)
- `add_grid_node()`: O(1)
- `finalize()`: O(r) where r = number of roots

Total scene construction: **O(n)** where n = number of nodes

### 10.2 Space Complexity

Arena usage:
- Per `Node`: `sizeof(Node)` + `sizeof(NodeHandle) * num_children`
- Per `TextNode`: `sizeof(TextNode)` (content not copied)
- Per `GridNode`: `sizeof(GridNode)` + `sizeof(NodeHandle) * num_children`
- Root storage: `sizeof(NodeHandle) * num_roots`

Total: **O(n)** where n = total nodes and edges

### 10.3 No Hidden Costs

- No virtual dispatch
- No dynamic casts
- No exception handling
- No RTTI lookups
- No string allocations
- No STL container growth

---

## 11. Versioning and Compatibility

### 11.1 Version 0.1.0

This specification describes AegisUI v0.1.0.

**Status: FROZEN**

No changes to this specification are permitted without a major version bump.

### 11.2 Semantic Versioning

- **Major version** change: Breaking changes to this specification
- **Minor version** change: Not applicable (API frozen)
- **Patch version** change: Bug fixes only, no spec changes

### 11.3 ABI Stability

**ABI stability is not guaranteed.**

Applications must recompile against new AegisUI versions.

---

## 12. Conformance

### 12.1 Implementation Requirements

A conforming AegisUI implementation must:
1. Follow all stage orderings in Section 2
2. Guarantee determinism per Section 6
3. Use only arena allocation per Section 7
4. Maintain O(n) complexity per Section 10

### 12.2 Application Requirements

A conforming application must:
1. Provide valid arena with sufficient capacity
2. Not use builder after finalization
3. Not mutate scene after finalization
4. Not access scene after arena reset

### 12.3 Testing

Conformance is verified via:
- Unit tests in `tests/` directory
- Determinism tests proving byte-identical output
- Allocation guard tests preventing heap usage
- Stress tests validating performance characteristics

---

## 13. References

This specification references:
- **[HLD.md](HLD.md)** — High-level design and philosophy
- **[LLD.md](LLD.md)** — Low-level implementation details
- **[SPEC_API.md](SPEC_API.md)** — Public API surface
- **[SPEC_WIDGET_LIFECYCLE.md](SPEC_WIDGET_LIFECYCLE.md)** — Widget lifecycle contract
- **[SPEC_INVARIANTS.md](SPEC_INVARIANTS.md)** — Determinism and memory guarantees

**Aegis Core specification** (external) defines layout and diffing behavior.

---

**END OF SPECIFICATION**
