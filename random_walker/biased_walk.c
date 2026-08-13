#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000
#define M 10000

int final_positions[M];
double mean_squared_positions[N+1];
double mean_positions[N+1]; // tracking <x> since it's no longer 0

int main() {
    srand(time(NULL));

    double p = 0.6; // 60% chance to go right and vice versa

    for (int step = 0; step <= N; step++) {
        mean_squared_positions[step] = 0.0;
        mean_positions[step] = 0.0;
    }

    for (int i = 0; i < M; i++) {
        int current_position = 0;

        for (int step = 1; step <= N; step++) {
            // generate a decimal threshold
            double random_percent = (double)rand() / RAND_MAX;

            int coin_flip;
            if (random_percent < p) {
                coin_flip = 1;
            } else {
                coin_flip = -1;
            }

            current_position += coin_flip;

            mean_positions[step] += (double)current_position;
            mean_squared_positions[step] += (double)(current_position * current_position);
        }
        final_positions[i] = current_position;
    }

    for (int step = 0; step <= N; step++) {
        mean_positions[step] /= M;
        mean_squared_positions[step] /= M;
    }

    FILE *fp1 = fopen("biased_means.csv", "w");
    fprintf(fp1, "Step,Mean,MeanSquared\n");
    for (int i = 0; i <= N; i++) {
        fprintf(fp1, "%d,%f,%f\n", i, mean_positions[i], mean_squared_positions[i]);
    }
    fclose(fp1);

    FILE *fp2 = fopen("biased_finals.csv", "w");
    fprintf(fp2, "FinalPosition\n");
    for (int i = 0; i < M; i++) {
        fprintf(fp2, "%d\n", final_positions[i]);
    }
    fclose(fp2);

    printf("Data saved cleanly to two files!\n");
}