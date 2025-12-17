# AegisUI Determinism & Memory Invariants Specification

**Version:** 0.1.0  
**Status:** FROZEN  
**Last Updated:** 2025-12-17

This document formally specifies the determinism and memory guarantees of AegisUI. This is a **normative specification** — implementations must conform to these rules.

---

## 1. Core Guarantee

**AegisUI provides byte-identical, deterministic scene construction.**

Given:
- Identical application state
- Identical builder method calls in identical order
- Sufficient arena capacity

Then:
- The resulting `Scene` is **byte-identical** across all executions
- This guarantee holds across:
  - Multiple runs on the same machine
  - Different machines with the same architecture
  - Different compilers (with same settings)
  - Different times

**This is not best-effort. This is a hard contract.**

---

## 2. Determinism Requirements

### 2.1 Input Determinism

For deterministic output, inputs must be deterministic:

**Application state:**
- All state used in scene construction must be reproducible
- No dependence on:
  - System time (unless explicitly captured in state)
  - Random number generators (unless seeded deterministically)
  - Uninitialized memory
  - Thread scheduling
  - Memory addresses

**Builder calls:**
- Same methods called in same order
- Same parameters (bitwise identical)
- Same arena (same size and alignment)

### 2.2 Output Determinism

AegisUI guarantees:

**Byte-identical scenes:**
- Same `Scene` struct
- Same root handles
- Same node allocation order in arena
- Same memory layout

**Byte-identical arena state:**
- Same allocation offset
- Same alignment padding
- Same memory contents

**Verifiable via:**
```cpp
std::uint64_t hash1 = hash_scene(scene1);
std::uint64_t hash2 = hash_scene(scene2);
assert(hash1 == hash2); // Must pass for deterministic inputs
```

### 2.3 Non-Deterministic Inputs

The following are **explicitly non-deterministic** and must not be used:

❌ `std::rand()` (unless seeded deterministically)  
❌ `std::time()` (unless value is part of application state)  
❌ Memory addresses (pointers as data)  
❌ Thread IDs  
❌ Uninitialized variables  
❌ Floating-point operations with rounding mode changes  

---

## 3. Memory Allocation Rules

### 3.1 Arena-Only Allocation

**Rule:** All scene construction allocations occur in the provided arena.

**Forbidden:**
- ❌ `new` / `delete`
- ❌ `malloc` / `free`
- ❌ STL containers that allocate (in hot path)
- ❌ `std::string` allocations
- ❌ Any heap allocation

**Allowed:**
- ✅ `arena.allocate()`
- ✅ Stack allocation (local variables)
- ✅ `std::span` (non-owning references)
- ✅ `std::string_view` (non-owning references)

### 3.2 Allocation Determinism

**Guarantee:** Allocations occur in **deterministic order**.

**Order:**
1. Allocations occur in the order of `add_*()` calls
2. Each allocation advances arena offset deterministically
3. Alignment padding is deterministic (8-byte minimum)

**Verification:**
```cpp
Arena arena1(buffer1);
Arena arena2(buffer2);
SceneBuilder b1(arena1);
SceneBuilder b2(arena2);

// Same calls
b1.add_node(...);
b2.add_node(...);

// Same offset
assert(arena1.offset() == arena2.offset());
```

### 3.3 Per-Frame Allocation Budget

**Budget:** All allocations fit in a single arena per frame.

**Size calculation:**
```
arena_size = sum(sizeof(NodeType)) + alignment_padding + root_storage
```

**No dynamic growth:**
- Arena is fixed-size
- No reallocation
- No fallback to heap

**Application responsibility:**
- Size arena appropriately for worst-case scene
- Monitor arena usage if needed
- Handle allocation failure (if arena too small)

---

## 4. Allocation Guarantees by Component

### 4.1 Node Allocation

**Cost per Node:**
```
sizeof(Node) + alignment_padding
```

**Determinism:**
- Always allocates in arena
- Always returns sequential handle
- Always advances offset deterministically

### 4.2 TextNode Allocation

**Cost per TextNode:**
```
sizeof(TextNode) + alignment_padding
```

**Content storage:**
- Content **not** allocated by AegisUI
- `std::string_view` is non-owning reference
- Application responsible for content lifetime

**Determinism:**
- TextNode allocation is deterministic
- Content pointer is **not** part of determinism (only content **value**)

### 4.3 GridNode Allocation

**Cost per GridNode:**
```
sizeof(GridNode) + alignment_padding
```

**Children storage:**
- Children **not** copied by AegisUI
- `std::span<const NodeHandle>` is non-owning reference
- Application responsible for children array lifetime

**Determinism:**
- GridNode allocation is deterministic
- Children pointer is **not** part of determinism (only children **values**)

### 4.4 Root Storage

**Cost:**
```
sizeof(NodeHandle) * num_roots + alignment_padding
```

**Allocation:**
- Occurs during `finalize()`
- Roots copied into arena
- Span references arena memory

**Determinism:**
- Root allocation is deterministic
- Root values determine scene hash

---

## 5. Ordering Guarantees

### 5.1 Node Registration Order

**Guarantee:** Nodes are registered in the order of `add_*()` calls.

**Handle assignment:**
```cpp
auto h1 = builder.add_node(...); // h1.value == 0
auto h2 = builder.add_node(...); // h2.value == 1
auto h3 = builder.add_node(...); // h3.value == 2
```

**Deterministic across runs:**
- Same calls → same handles
- Same order → same registration

### 5.2 Arena Allocation Order

**Guarantee:** Arena allocations occur in registration order.

**Layout:**
```
[Node 0] [padding] [Node 1] [padding] [Node 2] [padding] ... [Roots]
```

**Deterministic:**
- Same allocation order
- Same padding
- Same final offset

### 5.3 Finalization Order

**Guarantee:** Roots are stored in the order provided to `finalize()`.

**Example:**
```cpp
NodeHandle roots[] = {h3, h1, h2};
Scene scene = builder.finalize(roots);

// scene.roots[0] == h3
// scene.roots[1] == h1
// scene.roots[2] == h2
```

**Deterministic:**
- Root order preserved
- No sorting or reordering

---

## 6. Replay Equivalence

### 6.1 Event Log Replay

**Guarantee:** Replaying an event log produces identical scenes.

**Event log format:**
```
Event 1: State change
Event 2: State change
...
Event N: State change
```

**Replay:**
```cpp
for (const auto& event : event_log) {
    apply_event(app_state, event);
    Scene scene = build_scene(app_state, arena);
    // scene is identical to original run
}
```

### 6.2 Frame-by-Frame Equivalence

**Guarantee:** Each replayed frame produces byte-identical scene.

**Verification:**
```cpp
std::uint64_t original_hash = hash_scene(original_scene);
std::uint64_t replayed_hash = hash_scene(replayed_scene);
assert(original_hash == replayed_hash);
```

### 6.3 Multi-Run Equivalence

**Guarantee:** Multiple runs of same event log produce identical results.

**Applications:**
- Deterministic testing
- Bug reproduction
- Session replay
- Compliance verification (e.g., crypto surveillance POC)

---

## 7. Floating-Point Constraints

### 7.1 Floating-Point Usage

AegisUI uses `float` for:
- Layout dimensions (`LayoutSpec::width_value`, `height_value`)
- Style dimensions (`Padding`, `Border::width`)

### 7.2 Determinism Requirements

**No computation:**
- AegisUI does **not** perform floating-point arithmetic
- Values are stored as-is
- No rounding or conversion (except trivial copies)

**Platform independence:**
- IEEE 754 assumed (standard on all modern platforms)
- No extended precision (x87) issues
- No rounding mode changes

### 7.3 Application Responsibility

**Application must ensure:**
- Deterministic floating-point state inputs
- No non-deterministic computations before scene construction
- Consistent rounding modes (if applicable)

**Example:**
```cpp
// Deterministic
float width = 100.0f;

// Non-deterministic (avoid)
float width = std::sin(timestamp); // timestamp may vary
```

### 7.4 Layout Computation (Aegis Core)

**AegisUI responsibility:**
- Store layout spec as-is
- Guarantee bit-identical storage

**Aegis Core responsibility:**
- Resolve layout deterministically
- Handle floating-point computation
- Specify rounding behavior (in Aegis Core spec)

---

## 8. Memory Safety Guarantees

### 8.1 No Heap Allocation

**Guarantee:** Scene construction performs **zero heap allocations**.

**Enforcement:**
- Allocation guard tests
- Static analysis
- Code review

**Test:**
```cpp
// See tests/allocation_guard_test.cpp
AllocationGuard guard;
SceneBuilder builder(arena);
builder.add_node(...);
Scene scene = builder.finalize(roots);
assert(guard.allocation_count() == 0);
```

### 8.2 No Memory Leaks

**Guarantee:** No memory leaks in scene construction.

**Reasoning:**
- All allocations in arena
- Arena owns all memory
- Arena reset frees all memory
- No per-allocation deallocation needed

**Verification:**
- Valgrind clean
- AddressSanitizer clean
- LeakSanitizer clean

### 8.3 No Use-After-Free

**Guarantee:** No use-after-free in conforming code.

**Preconditions:**
- Scene not accessed after arena reset
- Spans/strings valid for frame lifetime

**Application responsibility:**
- Do not access scene after `arena.reset()`
- Ensure children/content spans outlive scene

### 8.4 No Buffer Overflows

**Guarantee:** No buffer overflows in arena allocation.

**Mechanism:**
- Arena bounds checking (returns `nullptr` on overflow)
- Assertions in debug builds
- No out-of-bounds writes

---

## 9. Thread Safety Guarantees

### 9.1 Single-Threaded Construction

**Requirement:** Scene construction is single-threaded.

**Non-thread-safe:**
- `SceneBuilder` methods
- `Arena` methods
- Node construction

**Application responsibility:**
- Do not call `add_*()` concurrently
- Do not share builder across threads

### 9.2 Immutable Scene Sharing

**Guarantee:** Finalized scene is thread-safe for reads.

**Safe operations:**
- Multiple threads reading same scene
- Concurrent hash computation
- Concurrent traversal (via Aegis Core)

**Unsafe operations:**
- ❌ Concurrent mutation (not possible, scene is immutable)
- ❌ Reading after arena reset

### 9.3 Arena Thread Safety

**Non-thread-safe:**
- `arena.allocate()`
- `arena.reset()`

**Application responsibility:**
- Use one arena per thread (if multi-threaded app)
- Do not share arena across threads

---

## 10. Platform Independence

### 10.1 Supported Platforms

**Architecture:**
- x86-64 (primary)
- ARM64 (supported)
- Any 64-bit architecture with 8-byte alignment

**Operating System:**
- Linux
- macOS  
- Windows
- Any OS with C++23 compiler

**Compiler:**
- GCC 13+
- Clang 18+

### 10.2 Platform-Specific Behavior

**Determinism across platforms:**
- ✅ Same scene hash on different OSes
- ✅ Same memory layout (given same struct layout)
- ⚠️ Struct layout may differ across compilers (ABI not frozen)

**Application responsibility:**
- Use same compiler for binary-level comparison
- Use same compilation flags

### 10.3 Endianness

**Assumption:** Little-endian (standard on x86-64, ARM64)

**Impact:**
- Memory layout is little-endian
- Hashes are little-endian
- No cross-endian guarantees

---

## 11. Determinism Testing

### 11.1 Required Tests

Implementations must include:

1. **Multi-run determinism**
   ```cpp
   Scene s1 = build_scene();
   Scene s2 = build_scene();
   assert(hash_scene(s1) == hash_scene(s2));
   ```

2. **Replay determinism**
   ```cpp
   auto hash1 = hash_scene(run_with_events(events));
   auto hash2 = hash_scene(run_with_events(events));
   assert(hash1 == hash2);
   ```

3. **Arena determinism**
   ```cpp
   Arena a1(buf1), a2(buf2);
   build_scene(a1);
   build_scene(a2);
   assert(a1.offset() == a2.offset());
   ```

4. **Allocation determinism**
   ```cpp
   AllocationGuard guard;
   build_scene();
   assert(guard.allocation_count() == 0);
   ```

### 11.2 Test Examples

See:
- `tests/determinism_test.cpp` — Multi-run determinism
- `tests/allocation_guard_test.cpp` — Zero heap allocations
- `tests/scene_hash_test.cpp` — Hash stability
- `poc/crypto_surveillance/` — Real-world replay testing

---

## 12. Performance Invariants

### 12.1 Time Complexity

**Guarantee:** O(n) scene construction where n = number of nodes.

**Breakdown:**
- `add_node()`: O(1)
- `add_text_node()`: O(1)
- `add_grid_node()`: O(1)
- `finalize()`: O(r) where r = number of roots

**No hidden costs:**
- No tree traversal
- No sorting
- No hash table lookups
- No dynamic dispatch

### 12.2 Space Complexity

**Guarantee:** O(n) space where n = total nodes and edges.

**Breakdown:**
- Node storage: O(nodes)
- Child references: O(edges)
- Root storage: O(roots)

**No hidden costs:**
- No temporary allocations
- No caching
- No auxiliary structures

### 12.3 Bounded Cost

**Guarantee:** All operations have **bounded, deterministic cost**.

**No unbounded operations:**
- ❌ Recursion (stack depth unbounded)
- ❌ Dynamic allocation (heap growth unbounded)
- ❌ Hash table rehashing (time spike unbounded)

**Application responsibility:**
- Control scene size
- Avoid pathological inputs

---

## 13. Invariant Enforcement

### 13.1 Compile-Time Enforcement

**Mechanisms:**
- POD types (no virtual functions)
- `noexcept` specifications
- `constexpr` where applicable
- `[[nodiscard]]` on all allocation functions

### 13.2 Runtime Enforcement

**Debug builds:**
- Assertions for API misuse
- Arena bounds checking
- Finalization state checks

**Release builds:**
- Minimal checks (performance critical)
- Undefined behavior on violations

### 13.3 Test Enforcement

**Continuous testing:**
- Determinism tests on every commit
- Allocation guard tests
- Performance regression tests

**CI pipeline:**
- Multiple runs to catch non-determinism
- Different machines/OSes
- Different compilers

---

## 14. Invariant Violations

### 14.1 How Violations Occur

**Common causes:**
- Non-deterministic application state
- Uninitialized memory
- Concurrent modification
- Floating-point non-determinism

### 14.2 Detection

**Symptoms:**
- Different scene hashes across runs
- Heap allocations in allocation guard tests
- Inconsistent arena offsets

**Tools:**
- `hash_scene()` comparison
- `AllocationGuard` tests
- Valgrind / ASan / TSan

### 14.3 Resolution

**Fix at source:**
1. Identify non-deterministic input
2. Make input deterministic (seed RNG, capture time in state, etc.)
3. Verify with determinism tests

---

## 15. Proof of Determinism

### 15.1 Crypto Surveillance POC

The **Crypto Trade Surveillance POC** (see `poc/crypto_surveillance/`) demonstrates:

- ✅ Byte-identical scenes across runs
- ✅ Perfect event log replay
- ✅ Zero heap allocations
- ✅ Deterministic performance (P99 < 10ms)

**Significance:** This POC proves properties **fundamentally impossible** in browser-based UI frameworks.

### 15.2 Formal Verification (Future)

**Potential approaches:**
- Model checking
- Symbolic execution
- Formal proofs of determinism

**Status:** Not implemented in v0.1, may be added in future versions.

---

## 16. Comparison with Non-Deterministic Systems

| Property | AegisUI | React/Flutter | Browser |
|----------|---------|---------------|---------|
| Scene determinism | ✅ Byte-identical | ❌ No guarantee | ❌ No guarantee |
| Allocation determinism | ✅ Arena-only | ❌ GC/heap | ❌ GC/heap |
| Replay equivalence | ✅ Perfect | ⚠️ Best-effort | ❌ Not possible |
| Floating-point | ✅ IEEE 754 | ⚠️ Varies | ⚠️ Varies |
| Thread safety | ✅ Read-only scene | ⚠️ Varies | ❌ Complex |

---

## 17. Future Invariant Extensions

### 17.1 Not Planned

❌ Probabilistic guarantees (determinism is absolute)  
❌ "Best-effort" determinism (defeats the purpose)  
❌ Optional determinism (always required)  

### 17.2 Potentially Compatible

⚠️ Formal verification of determinism  
⚠️ Extended platform support (RISC-V, etc.)  
⚠️ ABI stability (not guaranteed in v0.1)  

---

## 18. Conformance

### 18.1 Implementation Requirements

A conforming implementation must:
1. Guarantee byte-identical scenes for identical inputs
2. Use only arena allocation
3. Maintain deterministic allocation order
4. Provide replay equivalence

### 18.2 Application Requirements

A conforming application must:
1. Provide deterministic inputs
2. Not use non-deterministic functions
3. Size arena appropriately
4. Not access scene after arena reset

### 18.3 Testing Requirements

A conforming implementation must pass:
1. Multi-run determinism tests
2. Allocation guard tests (zero heap)
3. Arena determinism tests
4. Replay equivalence tests

---

## 19. References

- **[SPEC_EXECUTION.md](SPEC_EXECUTION.md)** — Execution model
- **[SPEC_API.md](SPEC_API.md)** — Public API surface
- **[SPEC_WIDGET_LIFECYCLE.md](SPEC_WIDGET_LIFECYCLE.md)** — Widget lifecycle
- **[HLD.md](HLD.md)** — High-level design
- **[LLD.md](LLD.md)** — Low-level design
- **POC Results:** `poc/crypto_surveillance/RESULTS.md`

---

**END OF SPECIFICATION**
