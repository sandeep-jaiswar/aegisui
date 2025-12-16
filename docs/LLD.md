# AegisUI — Low-Level Design (LLD)

## 1. Module Layout

```
aegisui/
 ├── include/aegis/ui/
 │    ├── ids.hpp
 │    ├── layout.hpp
 │    ├── style.hpp
 │    ├── node.hpp
 │    ├── scene.hpp
 │    └── scene_builder.hpp
 ├── src/
 ├── tests/
 └── docs/
```

---

## 2. Frame-Level Data Flow

```
Arena reset
   ↓
SceneBuilder created
   ↓
UI(State) invoked
   ↓
Nodes allocated in arena
   ↓
Scene finalized (immutable)
```

No object survives beyond the arena lifetime.

---

## 3. Memory Model

* All allocations use caller-provided arenas
* No heap allocation during scene build
* `SceneBuilder` is single-use per frame
* All spans reference arena memory

---

## 4. Identity Model

### NodeId

* Provided by application
* Stable across frames
* Used for diffing and event targeting

### NodeHandle

* Internal index into arena storage
* Frame-local
* Never exposed outside `Scene`

---

## 5. Layout Model

* Layout is declarative intent only
* Supported sizing:

  * Fixed
  * Flex

No content-based sizing.
No percentages.
No auto rules.

Layout resolution happens **only** in Aegis Core.

---

## 6. Styling Model

* Styles are fully resolved structs
* No inheritance
* No cascading
* No runtime computation

Style lookup must be constant-time.

---

## 7. Scene Immutability

Once built:

* No mutation allowed
* No child reordering
* No incremental updates

This enables safe diffing and replay.

---

## 8. Error Handling

* No exceptions
* Debug-time validation via assertions
* Release builds assume correctness

Invalid inputs are programmer errors.

---

## 9. Testing Requirements

Tests must prove:

* Deterministic output
* Zero allocations during build
* Stable NodeId mapping
* Arena reset correctness

No visual or snapshot tests belong here.

---
