#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITER 1000

int main(int argc, char *argv[]) {

    if (argc != 2) {
        return 1;
    }

    int N = atoi(argv[1]);

    int size = N + 2;

    int *old = malloc(size * sizeof(int));
    int *new = malloc(size * sizeof(int));

    for (int i = 1; i <= N; i++) {
        old[i] = rand() % 2;
    }

    clock_t start = clock();

    for (int iter = 0; iter < ITER; iter++) {

        old[0]   = old[N];
        old[N+1] = old[1];

        for (int i = 1; i <= N; i++) {

            if (old[i] == 1) {
                if (old[i+1] == 1)
                    new[i] = 1;
                else
                    new[i] = 0;
            }
            else {
                if (old[i-1] == 1)
                    new[i] = 1;
                else
                    new[i] = 0;
            }
        }

        for (int i = 1; i <= N; i++) {
            old[i] = new[i];
        }
    }

    clock_t end = clock();

    double tiempo = (double)(end - start) / CLOCKS_PER_SEC;

    printf("%.6f\n", tiempo);

    free(old);
    free(new);
    return 0;
}
