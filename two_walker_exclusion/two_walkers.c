#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "mersenne.h"

#define N 1000
#define M 1000000

double mean_x1[N+1];
double mean_x2[N+1];
double mean_x1_x2[N+1];

int main() {
    // Start the mersenne script
    seed_mt(time(NULL));

    // Clear memmory arrays to absolute 0
    for (int step = 0; step <= N; step++) {
        mean_x1[step] = 0.0;
        mean_x2[step] = 0.0;
        mean_x1_x2[step] = 0.0;
    }

    // Run M parallel trajectories 
    for (int i = 0; i < M; i++) {
        int pos1 = 0; // Position of walker 1
        int pos2 = 0; // Position of walker 2

        for (int step = 1; step <= N; step++) {
            // Flip for walker one using mersenne fn
            double roll1 = mt_rand_double();
            int flip1 = (roll1 < 0.5) ? 1 : -1;
            pos1 += flip1;

            // Flip for walker 2 using mersenne fn
            double roll2 = mt_rand_double();
            int flip2 = (roll2 < 0.5) ? 1 : -1;
            pos2 += flip2;

            // Add values to our cumulative step buckets
            mean_x1[step] += (double)pos1 / M;
            mean_x2[step] += (double)pos2 / M;
            mean_x1_x2[step] += (double)(pos1 * pos2) / M;
        }
    }

    // Computing the final statistics and writing to a csv file
    FILE *fp = fopen("two_walkers_correlation.csv", "w");
    fprintf(fp, "Step,MeanX1,MeanX2,MeanX1X2,Correlation\n");

    for (int step = 0; step <= N; step++) {

        double correlation = mean_x1_x2[step] - (mean_x1[step] * mean_x2[step]);

        fprintf(fp, "%d,%f,%f,%f,%f\n", step, mean_x1[step], mean_x2[step], mean_x1_x2[step], correlation);
    }
    fclose(fp);

    printf("Task 3(i) completed using mersenne.h module!\n");
    return 0;
}

