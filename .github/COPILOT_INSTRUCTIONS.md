# AegisUI – Copilot Instructions

> **Audience**: AI coding assistants, contributors, reviewers
>
> **Mindset Required**: Senior / Principal-level C++23 systems engineer

This document defines **how code must be written** in the `aegisui` repository.
If you violate these rules, the change is invalid — even if it works.

---

## 1. What AegisUI Is

**AegisUI** is a *pure, deterministic UI description framework*.

It exists to:

* Convert **application state → immutable scene graphs**
* Provide **typed UI construction primitives**
* Enforce **predictable, bounded, replayable behavior**

It is **not**:

* A runtime
* A renderer
* A browser
* A widget toolkit
* A scripting environment

AegisUI produces **data**, not effects.

---

## 2. Core Vision

> Replace the accidental complexity of the modern web UI stack with a **deterministic, compile-time, systems-level UI model**.

AegisUI assumes:

* Performance is a *design constraint*, not an optimization phase
* Determinism is more valuable than flexibility
* Explicit data beats implicit behavior

We are not competing with React, HTML, or CSS.
We are **removing the problem they exist to solve**.

---

## 3. Fundamental Principles (Non‑Negotiable)

### 3.1 Determinism First

* Same inputs must produce **byte-identical scenes**
* No hidden state
* No ambient context

### 3.2 Purity

* UI is a pure function of state
* No side effects
* No callbacks
* No observers

### 3.3 Explicitness

* All layout intent is explicit
* All styles are fully resolved
* All memory ownership is obvious

### 3.4 Bounded Cost

* O(n) scene construction
* No unbounded recursion
* No hidden allocations

---

## 4. What You MUST Assume as a Contributor

You must think like:

* A compiler engineer
* A low‑latency trading systems developer
* A formal methods reviewer

Not like:

* A frontend developer
* A framework consumer
* A UX prototyper

If you optimize for convenience, you are doing it wrong.

---

## 5. Coding Rules (Hard Constraints)

### Memory

* ❌ No `new`, `delete`, `malloc`
* ❌ No owning STL containers in hot paths
* ✅ All allocations go through provided arenas

### Language Features

* ❌ No exceptions
* ❌ No RTTI
* ❌ No virtual functions
* ❌ No dynamic polymorphism
* ✅ Value types and PODs preferred

### APIs

* ❌ No string‑based identifiers
* ❌ No implicit defaults
* ❌ No global state
* ✅ All behavior must be explicit in types

---

## 6. Architectural Boundaries

### Allowed Dependencies

* Standard C++23 library (subset)
* `aegis-core` headers **only via stable interfaces**

### Forbidden Dependencies

* OS headers
* GPU / rendering APIs
* Windowing systems
* Threads, async, timers

If a feature needs these — it does not belong in AegisUI.

---

## 7. Documentation as Law

All implementation must conform to:

* **HLD**: `docs/HLD.md`
* **LLD**: `docs/LLD.md`
* **NON_GOALS**: `docs/NON_GOALS.md`

If code and docs disagree:

> **The docs win. Always.**

---

## 8. Review Checklist (Mental Model)

Before writing or suggesting code, ask:

1. Is this deterministic?
2. Is this explicit?
3. Is this bounded in time and memory?
4. Can this be replayed exactly?
5. Does this leak policy upward or downward?

If any answer is "no" — stop.

---

## 9. What Success Looks Like

AegisUI is successful if:

* It can build a high‑density, interactive UI
* It runs for days without drift or leaks
* It is boring to debug
* It feels more like a compiler than a framework

---

## 10. Final Rule

> **If you cannot explain a change in terms of determinism, cost, and invariants — it does not belong in this repository.**

This project exists to prove that UI can be:

* Fast
* Correct
* Predictable

Not convenient.

---

**Read the HLD and LLD before writing a single line of code.**
