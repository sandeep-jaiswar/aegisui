# Contributing to AegisUI

Thank you for your interest in contributing to AegisUI.

**Before proceeding**: This is not a typical open-source project. Please read this document **in full** before submitting any contribution.

---

## ⚠️ API Freeze Notice

**AegisUI v0.1 is FROZEN as of 2025-12-17.**

**No new features, widgets, or API changes are permitted until formal unfreezing.**

All contributions must conform to the **frozen specifications**:
- **[SPEC_EXECUTION.md](docs/SPEC_EXECUTION.md)** — Execution model
- **[SPEC_API.md](docs/SPEC_API.md)** — Public API surface (FROZEN)
- **[SPEC_WIDGET_LIFECYCLE.md](docs/SPEC_WIDGET_LIFECYCLE.md)** — Widget lifecycle
- **[SPEC_INVARIANTS.md](docs/SPEC_INVARIANTS.md)** — Determinism and memory guarantees

**Acceptable contributions during freeze:**
- Bug fixes that maintain spec compliance
- Test additions
- Documentation improvements
- Performance optimizations that preserve semantics

**Rejected during freeze:**
- New widgets or node types
- API additions or changes
- New features
- Breaking changes

---

## Prerequisites

### You MUST Read These First

Before writing **any code**, you must read and understand:

1. **[docs/SPEC_EXECUTION.md](docs/SPEC_EXECUTION.md)** — Execution Model Specification (FROZEN)
2. **[docs/SPEC_API.md](docs/SPEC_API.md)** — Public API Specification (FROZEN)
3. **[docs/SPEC_WIDGET_LIFECYCLE.md](docs/SPEC_WIDGET_LIFECYCLE.md)** — Widget Lifecycle Specification (FROZEN)
4. **[docs/SPEC_INVARIANTS.md](docs/SPEC_INVARIANTS.md)** — Determinism & Memory Invariants (FROZEN)
5. **[docs/HLD.md](docs/HLD.md)** — High-Level Design
6. **[docs/LLD.md](docs/LLD.md)** — Low-Level Design  
7. **[docs/NON_GOALS.md](docs/NON_GOALS.md)** — What we will never do
8. **[.github/COPILOT_INSTRUCTIONS.md](.github/COPILOT_INSTRUCTIONS.md)** — Coding standards and mindset

If you submit code that contradicts these documents, your contribution **will be rejected** — even if it works.

---

## What We Value

AegisUI prioritizes:

* **Determinism** over dynamism
* **Explicitness** over convenience
* **Predictability** over flexibility
* **Bounded cost** over expressiveness

If your contribution optimizes for developer convenience at the expense of these principles, it does not belong here.

---

## Rejection Criteria

Your contribution **will be rejected** if it:

### Violates Frozen Specifications

* ❌ Adds new public APIs not in [SPEC_API.md](docs/SPEC_API.md)
* ❌ Changes frozen API signatures or semantics
* ❌ Violates execution model in [SPEC_EXECUTION.md](docs/SPEC_EXECUTION.md)
* ❌ Breaks widget lifecycle in [SPEC_WIDGET_LIFECYCLE.md](docs/SPEC_WIDGET_LIFECYCLE.md)
* ❌ Violates invariants in [SPEC_INVARIANTS.md](docs/SPEC_INVARIANTS.md)

### Violates Core Principles

* ❌ Breaks determinism (non-identical output for identical input)
* ❌ Introduces unbounded operations (recursion, hidden allocations)
* ❌ Adds implicit behavior or hidden state
* ❌ Uses forbidden language features (exceptions, RTTI, virtuals)

### Contradicts Design Constraints

* ❌ Adds dependencies on OS, GPU, or windowing systems
* ❌ Implements features listed in [docs/NON_GOALS.md](docs/NON_GOALS.md)
* ❌ Makes scene construction non-O(n)
* ❌ Introduces global state or ambient context

### Violates Code Standards

* ❌ Uses `new`, `delete`, or `malloc` instead of arenas
* ❌ Uses string-based identifiers
* ❌ Adds STL containers in hot paths without justification
* ❌ Ignores [.github/COPILOT_INSTRUCTIONS.md](.github/COPILOT_INSTRUCTIONS.md)

### Quality Issues

* ❌ Lacks tests proving determinism
* ❌ Changes behavior without updating documentation
* ❌ Adds features without clear use cases in the HLD/LLD

---

## Required Mindset

You must think like:

* ✅ A compiler engineer
* ✅ A low-latency systems developer  
* ✅ A formal methods reviewer

**Not like**:

* ❌ A frontend developer
* ❌ A framework consumer
* ❌ A UX prototyper

If you are optimizing for "ease of use" or "developer experience", you are approaching this project incorrectly.

---

## Contribution Workflow

### 1. Discuss First (for features)

Open an issue **before** writing code for:

* New APIs or node types
* Changes to the scene building model
* Performance optimizations that change semantics

Proposals must demonstrate alignment with:

* The determinism contract
* The bounded-cost requirement  
* The existing HLD/LLD

### 2. Write Code

All code must:

* Follow [.github/COPILOT_INSTRUCTIONS.md](.github/COPILOT_INSTRUCTIONS.md)
* Match existing code style (use `.clang-format` and `.clang-tidy`)
* Be deterministic and explicitly testable as such
* Avoid allocations outside of provided arenas

### 3. Write Tests

Every change must include tests that prove:

* ✅ Deterministic output (same input → same output)
* ✅ Memory safety (no leaks, no invalid accesses)
* ✅ API correctness

Tests must **not** depend on:

* ❌ Visual output or screenshots
* ❌ Timing or threads
* ❌ External state or files

### 4. Update Documentation

If your change affects:

* Public APIs → update [docs/SPEC_API.md](docs/SPEC_API.md) (only if unfrozen)
* Execution model → update [docs/SPEC_EXECUTION.md](docs/SPEC_EXECUTION.md) (only if unfrozen)
* Widget lifecycle → update [docs/SPEC_WIDGET_LIFECYCLE.md](docs/SPEC_WIDGET_LIFECYCLE.md) (only if unfrozen)
* Invariants → update [docs/SPEC_INVARIANTS.md](docs/SPEC_INVARIANTS.md) (only if unfrozen)
* Design philosophy → update [docs/HLD.md](docs/HLD.md)
* Implementation details → update [docs/LLD.md](docs/LLD.md)  
* Scope decisions → update [docs/NON_GOALS.md](docs/NON_GOALS.md)

**During API freeze:** Spec changes require major version bump and are generally rejected.

**Documentation is law.** If code and docs disagree, the docs win.

### 5. Submit a Pull Request

In your PR description, explain:

1. **What** you changed (the technical change)
2. **Why** it aligns with AegisUI's principles
3. **How** you verified determinism and correctness

PRs without this context will be rejected.

---

## Review Standards

Reviewers will ask:

1. **Is this deterministic?**  
   Can we replay this and get identical results?

2. **Is this explicit?**  
   Are all behaviors clear from the types and signatures?

3. **Is this bounded?**  
   Can we prove O(n) cost and no hidden allocations?

4. **Does this belong here?**  
   Or should it live in Aegis Core / application code?

If you cannot answer these questions clearly, your PR is not ready.

---

## What We Look For

**Good contributions**:

* Fix actual bugs in determinism or memory safety
* Add missing tests for existing behavior
* Improve documentation clarity
* Optimize hot paths without breaking semantics

**Bad contributions**:

* Add convenience APIs that hide complexity
* Introduce implicit defaults or magic behavior
* Request features from [docs/NON_GOALS.md](docs/NON_GOALS.md)
* Change APIs without consulting HLD/LLD

---

## Communication

* **Issues**: For bug reports, feature discussions, and questions
* **Pull Requests**: Only for concrete, ready-to-merge changes
* **Discussions**: For open-ended questions about design

Do not open PRs for half-baked ideas. Use issues or discussions first.

---

## Code of Conduct

We are here to build a deterministic, high-performance UI framework.

* Be respectful, but direct
* Focus on technical merit, not personalities  
* Accept that "no" is a valid answer

If you cannot separate critique of your code from critique of yourself, this project may not be a good fit.

---

## Final Note

> **If you cannot explain your change in terms of determinism, bounded cost, and explicit invariants — it does not belong in AegisUI.**

AegisUI exists to prove that UI can be:

* Fast
* Correct  
* Predictable

**Not** convenient.

If you are aligned with this vision, we welcome your contributions.

If not, that's okay — there are many other projects that optimize for different goals.

---

**Read the docs. Think deeply. Contribute deliberately.**
