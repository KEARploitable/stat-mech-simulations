#ifndef MERSENNE_H
#define MERSENNE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// MT19937 Constants 
#define MT_N 624
#define MT_M 397
#define MATRIX_A 0x9908b0dfUL   // constant vector a
#define UPPER_MASK 0x80000000UL // most significant w-r bits
#define LOWER_MASK 0x7fffffffUL // least significant r bits

static uint32_t mt[MT_N]; // The state vector array
static int mti = MT_N + 1; // mti==MT_N+1 meand mt[MT_N] is not initisalised

// 1. INITIALIZATION - Fills the 624 slots using a seed
void seed_mt(uint32_t s) {
    mt[0] = s & 0xffffffffUL;
    for (mti = 1; mti < MT_N; mti++) {
        mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
        mt[mti] &= 0xffffffffUL; // for >32 bit machines
    }
}

// 2. TWIST - Refreshes the numbers when the array is filled
void twist_mt(void) {
    uint32_t y;
    static uint32_t mag01[2] = {0x0UL, MATRIX_A};
    int kk;

    for (kk = 0; kk < MT_N - MT_M; kk++) {
        y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
        mt[kk] = mt[kk + MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (; kk < MT_N - 1; kk++) {
        y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
        mt[kk] = mt[kk + (MT_M - MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[MT_N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
    mt[MT_N - 1] = mt[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

    mti = 0; // Reset index counter
}

// 3. TEMPERING - Scrambles the bits to extract a number
uint32_t mt_rand(void) {
    uint32_t y;

    if (mti >= MT_N) { // if seed_mt() has not been called, a default initial seed is used
        if (mti == MT_N + 1) seed_mt(5489UL); 
        twist_mt();
    }

    y = mt[mti++];

    // Tempering transformations
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

// Helper function to get a clean double between 0.0 and 1.0
double mt_rand_double(void) {
    return (double)mt_rand() / 4294967295.0; // 4294967295 is max value of uint32_t
}

#endif