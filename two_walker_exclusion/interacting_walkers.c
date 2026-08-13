#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "mersenne.h"

#define N 1000
#define M 100000

double mean_x1[N+1];
double mean_x2[N+1];
double mean_x1_x2[N+1];

int main() {
    seed_mt(time(NULL));

    for (int step = 0; step <= N; step++) {
        mean_x1[step] = 0.0;
        mean_x2[step] = 0.0;
        mean_x1_x2[step] = 0.0;
    }

    for (int i = 0; i < M; i++) {
        // The two walkers cannot start at the same position!
        int pos1 = 1;
        int pos2 = -1;

        // Record step 0 intitial before loops

        mean_x1[0] += (double)pos1;
        mean_x2[0] += (double)pos2;
        mean_x1_x2[0] += (double)(pos1 * pos2);

        for (int step = 1; step <= N; step++) {
            // flip for walker 1
            int flip1 = (mt_rand_double() < 0.5) ? 1 : -1;
            int next_pos1 = pos1 + flip1;

            // flip for walker 2
            int flip2 = (mt_rand_double() < 0.5) ? 1 : -1;
            int next_pos2 = pos2 + flip2;

            // Check for collisions or crossing over!
            // Condition: Next positions are legal only if walker 1 stays to the right of walker 2
            if (next_pos1 > next_pos2) {
                // move is safe and coordinates can be updated
                pos1 = next_pos1;
                pos2 = next_pos2;
            } else {
                // they collide and we don't change their positions for this step
            }

            mean_x1[step] += (double)pos1;
            mean_x2[step] += (double)pos2;
            mean_x1_x2[step] += (double)(pos1 * pos2);
        }
    }

    FILE *fp = fopen("Interacting_correlations.csv", "w");
    fprintf(fp, "Step,MeanX1,MeanX2,MeanX1X2,Correlation\n");

    for (int step = 0; step <= N; step++) {
        mean_x1[step] /= M;
        mean_x2[step] /= M;
        mean_x1_x2[step] /= M;

        double correlation = mean_x1_x2[step] - (mean_x1[step] * mean_x2[step]);
        fprintf(fp, "%d,%f,%f,%f,%f\n", step, mean_x1[step], mean_x2[step], mean_x1_x2[step], correlation);
    }
    fclose(fp);

    printf("Task 3(ii) completed! Data saved to interacting_correlation.csv\n");
    return 0;
}