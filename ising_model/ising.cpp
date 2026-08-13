#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <fstream>

using namespace std;

struct SimulationConfig {
    const int L = 30;
    const double J = 1.0;
    const double Temperature = 2;
};

class IsingModel {
private:
    SimulationConfig config;
    vector<vector<int>> grid;
    mt19937 rng;

public:
    IsingModel(unsigned int seed) : rng(seed) {
        grid.resize(config.L, vector<int>(config.L));
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

        return total_energy / 2.0;
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

int main() {
    unsigned int system_seed = static_cast<unsigned int>(time(nullptr));

    IsingModel simulation(system_seed);

    cout << "Starting Simulation..." << endl;
    cout << "Intial Grid Energy: " << simulation.calculateTotalEnergy() << endl;
    
    ofstream movieFile("ising_frames.csv");

    int total_sweeps = 1000;
    for (int sweep = 1; sweep <= total_sweeps; ++sweep) {
        simulation.runFullSweep();

        // every 10 frames we take a camera snapshot of the layout
        if (sweep % 10 == 0) {
            simulation.saveGridState(movieFile);
        }
    }
    movieFile.close();

    cout << "Simulation Completed! 100 frames saved to ising_frames.csv" << endl;
    cout << "Final grid energy: " << simulation.calculateTotalEnergy() << endl;

    return 0;
}