//Version 1 - Sequential version

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITER 1000

void init_random(int *old, int N) {
    for (int i = 1; i <= N; i++) {
        old[i] = rand() % 2;
    }
}

void apply_periodic_boundaries(int *old, int N) {
    old[0]   = old[N];
    old[N+1] = old[1];
}

void update_cells(const int *old, int *new, int N) {
    for (int i = 1; i <= N; i++) {
        if (old[i] == 1) {
            new[i] = (old[i+1] == 1) ? 1 : 0;
        } else {
            new[i] = (old[i-1] == 1) ? 1 : 0;
        }
    }
}

void copy_array(const int *src, int *dst, int N) {
    for (int i = 1; i <= N; i++) {
        dst[i] = src[i];
    }
}

double run_simulation(int N) {
    int size = N + 2;

    int *old = malloc(size * sizeof(int));
    int *new = malloc(size * sizeof(int));

    init_random(old, N);

    clock_t start = clock();

    for (int iter = 0; iter < ITER; iter++) {
        apply_periodic_boundaries(old, N);
        update_cells(old, new, N);
        copy_array(new, old, N);
    }

    clock_t end = clock();

    free(old);
    free(new);

    return (double)(end - start) / CLOCKS_PER_SEC;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    int N = atoi(argv[1]);

    double tiempo = run_simulation(N);

    printf("%.6f\n", tiempo);

    return 0;
}
