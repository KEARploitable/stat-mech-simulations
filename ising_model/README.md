# High-Performance 2D Ising Model & Finite-Size Scaling

A high-performance C++ simulation engine for the 2D square-lattice Ising model, built to study phase transitions, compute macroscopic thermodynamic observables, and extract critical exponents via Finite-Size Scaling (FSS).

This repository traces a progression from a standard Metropolis-Hastings Markov Chain Monte Carlo (MCMC) implementation to a cache-optimized Wolff cluster algorithm, built specifically to defeat critical slowing down near the phase transition.

---

## Core Architecture

The codebase separates the physics engine from the experimental drivers:

| File | Purpose |
|---|---|
| `IsingModel.hpp` | Core C++ engine — grid memory management, thermodynamic calculators (energy, magnetization), and update algorithms |
| `phase1_validation.cpp` | Generates dynamic grid snapshots for visualization; validates equilibrium states against Onsager's exact analytical solution |
| `phase2_fss.cpp` | Multi-lattice driver spanning $L \in \{16, 32, 64, 128\}$ across the critical region; measures specific heat, susceptibility, and the Binder cumulant |
| `phase3_wolff.cpp` | Cluster-update driver replacing local spin-flips, targeting $O(1)$ thermalization scaling near $T_c$ |

---

## Phase 1–2: Validation & Finite-Size Scaling

The simulation is first validated against Onsager's exact 2D solution, then run across four lattice sizes to extract critical exponents via finite-size scaling.

The standard Metropolis algorithm successfully reproduces the phase transition, including the classic divergence of magnetic susceptibility $\chi$ and specific heat $C_v$ as $L$ increases. Using log-log regression on peak susceptibility vs. $L$, and on magnetization at $T_c \approx 2.269$, the extracted critical exponents are:

| Exponent | Measured (Metropolis) | Exact (2D Ising) |
|---|---|---|
| $\gamma/\nu$ | 1.712 | 1.75 |
| $\beta/\nu$ | 0.1427 | 0.125 |

### The Metropolis Bottleneck: Critical Slowing Down

As $T \to T_c$, the correlation length $\xi$ diverges and macroscopic domains of aligned spins form. Because Metropolis updates one spin at a time, the probability of flipping a spin embedded deep inside a large domain drops toward zero — causing the autocorrelation time to scale as

$$\tau \propto L^z, \quad z \approx 2.16$$

**Methodological note:** this exponential growth in relaxation time limits how many statistically independent measurements are achievable at large $L$ (e.g. $L=128$) within practical single-run compute budgets. This insufficient measurement depth — not a flaw in the underlying scaling theory — is the primary driver of the ~14% deviation in the measured $\beta/\nu$ from its exact value. This exact bottleneck is the motivation for Phase 3.

---

## Phase 3: Wolff Cluster Algorithm

To address critical slowing down directly, the engine implements the Wolff single-cluster algorithm. Rather than proposing single-spin flips, it grows a cluster of aligned spins via a bond-addition probability

$$P_{\text{add}} = 1 - e^{-2J/T}$$

traversed with breadth-first search (BFS), and flips the entire cluster in one step with 100% acceptance.

### Implementation notes (mechanical sympathy)

- The BFS queue uses a flat, pre-allocated `std::vector` rather than a fragmented `std::queue`, to maximize L1 cache hits during cluster growth.
- Visited states are tracked in-place by flipping spins at enqueue time, eliminating the need for a separate $O(N)$ boolean visited-matrix.

### Result

| Exponent | Measured (Wolff) | Exact (2D Ising) |
|---|---|---|
| $\gamma/\nu$ | 1.691 | 1.75 |
| $\beta/\nu$ | 0.133 | 0.125 |

The cluster approach reduces the dynamical critical exponent to roughly $z \approx 0.25$, generating statistically independent configurations far more rapidly even directly at $T_c$ — closing the large-lattice measurement gap and bringing $\beta/\nu$ within 6% of the exact value, in substantially less compute time than the Metropolis run required.

---

## Known Limitations

- **Metropolis $\beta/\nu$ deviation (~14%):** attributed to insufficient measurement statistics at $L=128$ under single-run compute constraints, not a methodological error — see above.
- **Binder cumulant / specific heat noise at $L=128$:** visible fluctuation artifacts at the largest lattice size point to the same root cause — more measurement sweeps are needed at this scale for both algorithms.
- $T_c$ is currently taken as the theoretical value (2.269) rather than extracted from the Binder-cumulant crossing point of the data itself; redoing the exponent fits with a data-derived $T_c$ is expected to tighten both $\beta/\nu$ results.
- No error bars (binning/jackknife) are included yet on the exponent fits or thermodynamic observables.

---

## Roadmap

- [ ] Extract $T_c$ from the Binder-cumulant crossing point and re-fit $\beta/\nu$ for both algorithms
- [ ] Rerun $L=128$ with extended measurement sweeps to resolve the Binder-cumulant/specific-heat noise
- [ ] Add binning/jackknife error bars to all fitted quantities
- [ ] **Autocorrelation time-series driver:** generate step-by-step magnetization data for both algorithms on an $L=64$ lattice at $T_c$, to directly plot and quantify the autocorrelation function $C(t)$ and empirically demonstrate the acceleration claim
