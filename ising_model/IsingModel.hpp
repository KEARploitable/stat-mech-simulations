#ifndef ISING_MODEL_HPP
#define ISING_MODEL_HPP

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>

using namespace std;

// 1. Dynamic Configuration
struct SimulationConfig {
    const int L;
    const double J = 1.0;
    double Temperature;

    // Constructor to set size and temp dynamically
    SimulationConfig(int size, double temp) : L(size), Temperature(temp) {}
};

class IsingModel {
private:
    SimulationConfig config;
    vector<vector<int>> grid;
    mt19937 rng;

public:
    // Update constructor to take L and T
    IsingModel(int L, double T, unsigned int seed) : config(L,T), rng(seed) {
        grid.resize(config.L, vector<int>(config.L));
        initializeRandomGrid();
    }

    void setTemperature(double new_temp) {
        config.Temperature = new_temp;
    }

    void resetGrid() {
        initializeRandomGrid();
    }

    void initializeRandomGrid() {
        uniform_real_distribution<double> dist(0.0, 1.0);

        for (int i = 0; i < config.L; ++i) {
            for (int j = 0; j < config.L; ++j) {
                grid[i][j] = (dist(rng) < 0.5) ? 1 : -1;
            }
        }
    }

    double calculateMagnetization() const {
        double total_spin = 0.0;
        for (int i=0; i<config.L; ++i) {
            for (int j=0; j<config.L; ++j) {
                total_spin += grid[i][j];
            }
        }
        return total_spin/(config.L*config.L);
    }

    double calculateTotalEnergy() const {
        double total_energy = 0.0;
        int L = config.L;

        for (int i = 0; i < L; ++i) {
            for (int j = 0; j < L; ++j) {
                int current_spin = grid[i][j];

                int neighbor_right = grid[i][(j+1)%L];
                int neighbor_below = grid[(i+1)%L][j];

                total_energy += -config.J * current_spin * neighbor_right;
                total_energy += -config.J * current_spin * neighbor_below;
            }
        }

        return (total_energy / 2.0) / (L * L);
    }

    void metropolisStep() {
        int L = config.L;

        uniform_int_distribution<int> rand_coord(0, L-1);
        int i = rand_coord(rng);
        int j = rand_coord(rng);
        
        int current_spin = grid[i][j];

        // check the spin of the neighbors 
        int up    = grid[(i - 1 + L) % L][j];
        int down  = grid[(i + 1) % L][j];
        int left  = grid[i][(j - 1 + L) % L];
        int right = grid[i][(j + 1) % L];

        // compute the energy change if the spin were flipped
        int neighbor_sum = up + down + left + right;
        double delta_E = 2.0 * config.J * current_spin * neighbor_sum;

        if (delta_E <= 0) {
            // flip the spin as it will lower or not affect the current config
            grid[i][j] = -current_spin;
        } else {
            uniform_real_distribution<double> rand_float(0.0, 1.0);
            double roll = rand_float(rng);
            
            // Boltzmann probability factor 
            if (roll < exp(-delta_E / config.Temperature)) {
                grid[i][j] = -current_spin;
            }
        }
    }

    void wolffStep() {
        // 1. Precompute cluster addition probability 
        double p_add = 1.0 - exp(-2.0 * config.J / config.Temperature);

        // 2. Pick up a random seed spin 
        uniform_int_distribution<int> rand_coord(0, config.L -1);
        int start_i = rand_coord(rng);
        int start_j = rand_coord(rng);

        int cluster_spin = grid[start_i][start_j];

        // 3. Initalise the flat BFS queue and the visited array
        // I think maybe the visited array can be done in place and the algorithm can be more optimised (please review later)

        // Flat contiguous array queue for L1 cache sympathy 
        vector<pair<int, int>> queue;
        queue.reserve(config.L * config.L);

        // Seed the BFS and flip IMMEDIATELY (In-place visited tracking)
        queue.push_back({start_i, start_j});
        grid[start_i][start_j] = -cluster_spin;

        int head = 0;
        uniform_real_distribution<double> rand_float(0.0, 1.0);

        while (head < queue.size()) {
            auto [curr_i, curr_j] = queue[head++];

            // define 4 neighbors (this can also be optimised more later)
            int neighbors[4][2] = {
                {(curr_i - 1 + config.L) % config.L, curr_j},
                {(curr_i + 1) % config.L, curr_j},
                {curr_i, (curr_j - 1 + config.L) % config.L},
                {curr_i, (curr_j + 1) % config.L}
            };

            for (int n = 0; n < 4; ++n) {
                int ni = neighbors[n][0];
                int nj = neighbors[n][1];

                // because flipped spins are no longer equal to the cluster spins
                // they are naturally skipped so we don't need any visited array 
                if (grid[ni][nj] == cluster_spin) {
                    if (rand_float(rng) < p_add) {
                        grid[ni][nj] = -cluster_spin;
                        queue.push_back({ni, nj});
                    }
                }
            }
        }

    }

    void runFullSweep() {
        int num_steps = config.L * config.L;
        for (int step = 0; step < num_steps; ++step) {
            metropolisStep();
        }
    }

    void saveGridState(ofstream& file) const {
        for (int i = 0; i < config.L; ++i) {
            for (int j = 0; j < config.L; ++j) {
                file << grid[i][j] << " ";
            }
        }
        file << "\n";
    }

    void displayStatus() const {
        cout << "C++ 2D Ising Lattice intialized successfully at size: "
        << config.L << "x" << config.L << endl;
    }
};

#endif