# Proof-of-Concept Demonstrations

This directory contains technical proof-of-concept demonstrations that validate AegisUI's core invariants in realistic scenarios.

## Available POCs

### [Crypto Trade Surveillance](crypto_surveillance/)

**Purpose**: Prove AegisUI can process high-volume real-time data with deterministic, bounded-cost performance.

**What it proves**:
- ✅ Determinism: Identical inputs → byte-identical outputs
- ✅ Bounded cost: O(n) scene construction
- ✅ Memory stability: Zero heap growth in steady-state
- ✅ Replay: Perfect event log replay capability
- ✅ Performance: P99 frame time < 16ms at scale

**Key Results**:
- 100k events processed
- P99 frame time: 13.4ms
- Memory stable at ~455KB
- Perfect determinism verified

**Run it**:
```bash
cmake -B build && cmake --build build
./build/crypto_surveillance_poc
```

See [crypto_surveillance/README.md](crypto_surveillance/README.md) for full details.

---

## POC Philosophy

These POCs exist to **prove technical invariants**, not to demonstrate products or UX.

### What POCs Are

- ✅ Technical capability demonstrations
- ✅ Benchmark harnesses
- ✅ Stress tests
- ✅ Determinism verifications
- ✅ Performance measurements

### What POCs Are NOT

- ❌ Product prototypes
- ❌ Feature demos
- ❌ UI/UX showcases
- ❌ Integration examples
- ❌ End-user applications

---

## Exit Criteria

Each POC has explicit **exit criteria** that must be met:

> An experienced systems engineer should be able to review the POC and conclude:
> **"This proves a property that is fundamentally impossible in alternative approaches."**

If a POC doesn't meet this bar, it is not included.

---

## Contributing a POC

To add a new POC:

1. Create a subdirectory under `poc/`
2. Include:
   - `README.md` - What is proven, what is omitted
   - `RESULTS.md` - Benchmark data and analysis
   - Source code with clear structure
   - CMake integration
3. Define explicit exit criteria
4. Prove properties that browsers/alternatives cannot achieve

See [crypto_surveillance/](crypto_surveillance/) as a reference implementation.
