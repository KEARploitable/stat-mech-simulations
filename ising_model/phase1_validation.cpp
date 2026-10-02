#include "IsingModel.hpp"

int main() {
    unsigned int system_seed = static_cast<unsigned int>(time(nullptr));

    IsingModel simulation(30, 2.0, system_seed);

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

    ofstream resultsFile("onsager_validation.csv");
    resultsFile << "Temperature,Avg_Abs_Magnetization\n";

    int N_eq = 3000;
    int N_meas = 2000;
    int sample_interval = 10;

    for (double T = 1.0; T <= 4.0; T += 0.05) {
        simulation.setTemperature(T);
        simulation.resetGrid();

        for (int sweep=0; sweep<N_eq; ++sweep) {
            simulation.runFullSweep();
        }

        double abs_magnetization_sum = 0.0;
        int active_measurements = 0;

        for (int sweep=0; sweep<N_meas; ++sweep) {
            simulation.runFullSweep();

            if (sweep % sample_interval == 0) {
                abs_magnetization_sum += abs(simulation.calculateMagnetization());
                active_measurements++;
            } 
        }

        double final_avg_abs_m = abs_magnetization_sum / active_measurements;
        resultsFile << T << "," << final_avg_abs_m << "\n";

        cout << "Completed T = " << T << "| Average |m| = " << final_avg_abs_m << endl;
    }

    resultsFile.close();
    cout << "\nValidation experiment finished! Results saved to onsager_validation.csv" << endl;

    return 0;
}