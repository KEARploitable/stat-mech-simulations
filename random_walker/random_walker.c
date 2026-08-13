#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000  // Number of steps 
#define M 10000  // Number of trajectories

// Global arrays to avoid overflowing the stack memory
int final_positions[M];
double mean_squared_positions[N+1];

int main() {
    // Seeding the random number generator using the current time
    srand(time(NULL));

    // Initialise our calculation arrays to zeros
    for (int step = 0; step <= N; step++) {
        mean_squared_positions[step] = 0.0;
    }

    // Start a timer to see how fast C is
    clock_t start_time = clock();

    // Run M trajectories
    for (int i = 0; i < M; i++) {
        int current_position = 0;

        // Record the starting position squared at step 0
        mean_squared_positions[0] += (current_position * current_position);

        for (int step = 1; step <= N; step++) {
            // coin flip: rand() % 2 gives 0 or 1
            int coin_flip = (rand() % 2) * 2 - 1;
            current_position += coin_flip;

            // Accumulate the squared position for this step
            mean_squared_positions[step] += (double)(current_position * current_position);
        }

        // Save the very last position for our histogram
        final_positions[i] = current_position;
    }

    // Divide by M to get the actual averages 
    for (int step = 0; step <= N; step++) {
        mean_squared_positions[step] /= M;
    }

    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Simulation completed in: %f seconds\n", time_taken);

    // --- SAVE MEAN SQUARED DATA ---
    FILE *fp1 = fopen("mean_squared.csv", "w");
    fprintf(fp1, "Step,MeanSquared\n");
    for (int i = 0; i <= N; i++) {
        fprintf(fp1, "%d,%f\n", i, mean_squared_positions[i]);
    }
    fclose(fp1);

    // --- SAVE HISTOGRAM DATA ---
    FILE *fp2 = fopen("final_positions.csv", "w");
    fprintf(fp2, "FinalPosition\n");
    for (int i = 0; i < M; i++) {
        fprintf(fp2, "%d\n", final_positions[i]);
    }
    fclose(fp2);

    printf("Data saved cleanly to two files!\n");

    return 0;
}