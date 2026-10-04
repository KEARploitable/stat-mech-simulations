# High-Performance 2D Ising Model & Finite-Size Scaling

A high-performance C++ simulation engine for the 2D square-lattice Ising model, built to study phase transitions, compute macroscopic thermodynamic observables, and extract critical exponents via Finite-Size Scaling (FSS).

This repository traces a progression from a standard Metropolis-Hastings Markov Chain Monte Carlo (MCMC) implementation to a cache-optimized Wolff cluster algorithm, built specifically to address critical slowing down near the phase transition.

---

## Core Architecture

The codebase separates the physics engine from the experimental drivers:

| File | Purpose |
|---|---|
| `IsingModel.hpp` | Core C++ engine — grid memory management, thermodynamic calculators (energy, magnetization), and update algorithms |
| `phase1_validation.cpp` | Generates dynamic grid snapshots for visualization; validates equilibrium states against Onsager's exact analytical solution |
| `phase2_fss.cpp` | Multi-lattice driver spanning L ∈ {16, 32, 64, 128} across the critical region; measures specific heat, susceptibility, and the Binder cumulant |
| `phase3_wolff.cpp` | Cluster-update driver replacing local spin-flips, aimed at faster decorrelation near T꜀ |

---

## Phase 1–2: Validation & Finite-Size Scaling

The simulation is first validated against Onsager's exact 2D solution, then run across four lattice sizes to extract critical exponents via finite-size scaling.

The standard Metropolis algorithm successfully reproduces the phase transition, including the classic divergence of magnetic susceptibility χ and specific heat Cᵥ as L increases. Using log-log regression on peak susceptibility vs. L, and on magnetization at T꜀ ≈ 2.269, the extracted critical exponents are:

| Exponent | Measured (Metropolis) | Exact (2D Ising) |
|---|---|---|
| γ/ν | 1.712 | 1.75 |
| β/ν | 0.1427 | 0.125 |

### The Metropolis Bottleneck: Critical Slowing Down

As T → T꜀, the correlation length ξ diverges and macroscopic domains of aligned spins form. Because Metropolis updates one spin at a time, the probability of flipping a spin embedded deep inside a large domain drops toward zero, so the autocorrelation time τ is expected to grow with lattice size as:

```
τ ∝ L^z
```

Single-spin-flip Metropolis dynamics is widely believed to belong to the Model-A dynamic universality class, for which the literature-reported value of the dynamical critical exponent for the 2D Ising model is **z ≈ 2.16** (see Roadmap — this project has not yet directly measured τ(L) or fit z from its own data; the value above is cited from prior literature for context, not an empirical result of this repository).

**Methodological note:** the exponential growth in relaxation time implied by this scaling limits how many statistically independent measurements are achievable at large L (e.g. L=128) within practical single-run compute budgets. This insufficient measurement depth — not a flaw in the underlying scaling theory — is a likely contributor to the ~14% deviation in the measured β/ν from its exact value. This bottleneck is the motivation for Phase 3.

---

## Phase 3: Wolff Cluster Algorithm

To address critical slowing down directly, the engine implements the Wolff single-cluster algorithm. Rather than proposing single-spin flips, it grows a cluster of aligned spins via a bond-addition probability:

```
P_add = 1 - exp(-2J / T)
```

traversed with breadth-first search (BFS), and flips the entire cluster in one step with 100% acceptance.

### Implementation notes (mechanical sympathy)

- The BFS queue uses a flat, pre-allocated `std::vector` rather than a fragmented `std::queue`, to maximize L1 cache hits during cluster growth.
- Visited states are tracked in-place by flipping spins at enqueue time, eliminating the need for a separate O(N) boolean visited-matrix.

### Result

| Exponent | Measured (Wolff) | Exact (2D Ising) |
|---|---|---|
| γ/ν | 1.691 | 1.75 |
| β/ν | 0.133 | 0.125 |

The cluster approach brings β/ν within 6% of the exact value (vs. ~14% for Metropolis), consistent with Wolff generating statistically independent configurations more efficiently near T꜀.

**On "z" for Wolff — an important distinction.** Unlike Metropolis, the Wolff algorithm has no physical-dynamics interpretation: real spins do not flip in large correlated clusters simultaneously, so there is no meaningful *physical* dynamical exponent to assign to it. What the MCMC literature does report, by analogy and using the same symbol, is an **algorithmic/sampling-efficiency exponent** describing how Wolff's *autocorrelation time* (not a physical relaxation time) scales with L — reported values are typically small, often cited around z ≈ 0.2–0.35 depending on the observable and dimension. This project has **not yet measured this quantity directly** (see Roadmap); it is not currently claimed as a result of this repository.

---

## Known Limitations

- **Metropolis β/ν deviation (~14%):** attributed to insufficient measurement statistics at L=128 under single-run compute constraints, not a methodological error — see above.
- **Binder cumulant / specific heat noise at L=128:** visible fluctuation artifacts at the largest lattice size point to the same root cause — more measurement sweeps are needed at this scale for both algorithms.
- T꜀ is currently taken as the theoretical value (2.269) rather than extracted from the Binder-cumulant crossing point of the data itself; redoing the exponent fits with a data-derived T꜀ is expected to tighten both β/ν results.
- No error bars (binning/jackknife) are included yet on the exponent fits or thermodynamic observables.
- **No autocorrelation time or dynamical exponent z has yet been measured by this project**, for either algorithm. The τ ∝ L^z scaling and both z values discussed above are cited from prior literature for context only, and should not be read as results produced by this code.

---

## Roadmap

- [ ] Extract T꜀ from the Binder-cumulant crossing point and re-fit β/ν for both algorithms
- [ ] Rerun L=128 with extended measurement sweeps to resolve the Binder-cumulant/specific-heat noise
- [ ] Add binning/jackknife error bars to all fitted quantities
- [ ] **Autocorrelation time-series driver:** generate step-by-step magnetization data for both algorithms on an L=64 lattice at T꜀, fit τ(L) across multiple lattice sizes, and directly measure z for each algorithm — this is the step needed before any z value can be reported as an actual result of this project
