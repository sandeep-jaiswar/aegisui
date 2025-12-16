# docs/HLD.md

# AegisUI — High-Level Design (HLD)

## 1. Purpose

**AegisUI** is a deterministic, statically-typed UI description framework that converts application state into an immutable scene graph consumable by the Aegis engine.

It exists to eliminate:

* Runtime interpretation
* Implicit UI state
* Unbounded layout and rendering costs
* Non-deterministic behavior common in browser-based UI stacks

---

## 2. System Positioning

```
Application (State + Logic)
        ↓
     AegisUI
(Scene Construction Only)
        ↓
     Aegis Core
(Layout, Diff, Scheduling)
        ↓
 Runtime / GPU Backend
```

**AegisUI is a pure library.**

It has no concept of:

* Time
* Threads
* IO
* OS events
* Rendering

---

## 3. Core Responsibilities

AegisUI **does**:

* Define UI node types
* Define layout intent structures
* Define style data structures
* Provide a deterministic scene construction API

AegisUI **does not**:

* Execute layout algorithms
* Perform hit-testing
* Dispatch events
* Animate or interpolate
* Manage lifetimes beyond a frame

---

## 4. Determinism Contract

Given:

* Identical application state
* Identical style sheet
* Identical event stream

Then:

* The generated `Scene` must be **byte-identical**

This is a **hard contract**, not best-effort.

---

## 5. Design Constraints

* Scene construction must be **O(n)**
* All memory must be externally owned
* No global state
* No implicit defaults

If a feature violates these constraints, it is rejected.

---

## 6. Versioning Philosophy

* v0.x: Experimental API, strict determinism
* v1.0: API frozen, behavioral compatibility guaranteed
* ABI stability is **not** a goal

---
