# AegisUI

**A pure, deterministic UI description framework for systems-level applications.**

AegisUI converts application state into immutable scene graphs. It is **not** a runtime, renderer, or widget toolkit.

---

## What This Is

AegisUI is a **compile-time, statically-typed UI construction library** that:

* Converts application state → immutable scene graphs
* Provides typed primitives for UI description
* Guarantees **deterministic, byte-identical output** for identical inputs
* Enforces **O(n) bounded cost** for scene construction
* Operates as a **pure function** with no side effects

Think of it as a **data structure builder**, not a framework.

---

## What This Is NOT

AegisUI explicitly **does not**:

* ❌ Execute layout algorithms (handled by Aegis Core)
* ❌ Render or draw anything
* ❌ Handle events, input, or OS interaction
* ❌ Manage animation or state transitions
* ❌ Provide HTML/CSS compatibility
* ❌ Support runtime mutation or scripting
* ❌ Include implicit defaults or magic behavior

If you're looking for React, Flutter, or a browser engine — **this is not it**.

---

## The Determinism Contract

**Hard guarantee**: Given identical inputs (application state, styles, events), AegisUI produces **byte-identical scenes**.

This is not best-effort. It is a **non-negotiable requirement**.

Why this matters:

* Reproducible bugs
* Replayable sessions
* Testable at the binary level
* Predictable performance

If a feature breaks determinism, it will be **rejected**.

---

## Non-Goals (Permanent)

The following are **explicitly out of scope** and will never be added:

* HTML or CSS compatibility
* JavaScript or scripting engines
* Runtime mutation of UI trees
* Animations or asynchronous effects
* Implicit layout, styling, or cascading rules
* String-based identifiers
* WYSIWYG tooling

See [docs/NON_GOALS.md](docs/NON_GOALS.md) for the complete list.

---

## Philosophy

> **Predictability over flexibility.**  
> **Explicitness over convenience.**  
> **Determinism over dynamism.**

AegisUI exists to replace the accidental complexity of modern web UI stacks with a systems-level, compile-time model.

We are not competing with existing frameworks.  
We are **removing the problem they exist to solve**.

---

## Documentation

**Required reading before contributing:**

- [High-Level Design (HLD)](docs/HLD.md) — Architecture and design principles
- [Low-Level Design (LLD)](docs/LLD.md) — Implementation details and module structure
- [Non-Goals](docs/NON_GOALS.md) — Explicitly out-of-scope features
- [CONTRIBUTING.md](CONTRIBUTING.md) — How to contribute to this project

---

## Quick Example

```cpp
// Application owns state
struct AppState {
    int counter;
};

// UI is a pure function of state
Scene build_ui(const AppState& state, Arena& arena) {
    SceneBuilder builder(arena);
    
    auto root = builder.container(NodeId{1});
    builder.text(NodeId{2}, root, std::format("Count: {}", state.counter));
    
    return builder.finalize();
}
```

No callbacks. No observers. No magic.  
Just data in, data out.

---

## Proof of Capability

See the **[Crypto Trade Surveillance POC](poc/crypto_surveillance/)** for a complete demonstration of:

- ✅ **Determinism**: Byte-identical scenes from identical inputs
- ✅ **Performance**: P99 frame time 6.2ms at 166k events/sec
- ✅ **Stability**: Zero heap growth over extended runs
- ✅ **Replay**: Perfect event log replay capability

This POC proves properties that are **fundamentally impossible** in browser-based UI frameworks.

---

## License

See [LICENSE](LICENSE) file for details.
