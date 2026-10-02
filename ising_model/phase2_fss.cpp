#include "IsingModel.hpp"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main() {
    unsigned int seed = 42; 
    vector<int> lattice_sizes = {16, 32, 64, 128};

    ofstream out_file("fss_measurements.csv");
    out_file << "L,T,Avg_E,Avg_E2,Avg_Abs_M,Avg_M2,Avg_M4\n";

    for (int L : lattice_sizes) {
        cout << "\nStarting Metropolis FSS for Lattice Size L = " << L << endl;
        
        // Initialize the model with the current lattice size
        IsingModel simulation(L, 2.0, seed);

        // Sweeping across the critical region
        for (double T = 2.0; T <= 2.6; T += 0.02) {
            simulation.setTemperature(T);
            
            // Metropolis requires heavy scaling: L^2 sweeps
            int N_eq = 10000;  
            int N_meas = 50000; 
            int sample_interval = 10;

            // Equilibration Phase
            for (int step = 0; step < N_eq; ++step) {
                // Assuming runFullSweep() does L^2 single-spin metropolis flips
                simulation.runFullSweep(); 
            }

            double sum_E = 0, sum_E2 = 0;
            double sum_abs_M = 0, sum_M2 = 0, sum_M4 = 0;
            int active_measurements = 0;

            // Measurement Phase
            for (int step = 0; step < N_meas; ++step) {
                simulation.runFullSweep();

                if (step % sample_interval == 0) {
                    double E = simulation.calculateTotalEnergy();
                    double M = simulation.calculateMagnetization();
                    double abs_M = abs(M);

                    sum_E += E;
                    sum_E2 += E * E;
                    sum_abs_M += abs_M;
                    sum_M2 += M * M;
                    sum_M4 += M * M * M * M;
                    
                    active_measurements++;
                }
            }

            double avg_E = sum_E / active_measurements;
            double avg_E2 = sum_E2 / active_measurements;
            double avg_abs_M = sum_abs_M / active_measurements;
            double avg_M2 = sum_M2 / active_measurements;
            double avg_M4 = sum_M4 / active_measurements;

            out_file << L << "," << T << "," 
                     << avg_E << "," << avg_E2 << "," 
                     << avg_abs_M << "," << avg_M2 << "," << avg_M4 << "\n";
                     
            cout << "L=" << L << " | T=" << T << " | <|M|>=" << avg_abs_M << endl;
        }
    }
    
    out_file.close();
    cout << "\nMetropolis FSS simulation complete!" << endl;
    return 0;
}