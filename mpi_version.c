#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define ITER 1000

void init_random(int *old, int local_N) {
    for (int i = 1; i <= local_N; i++) {
        old[i] = rand() % 2;
    }
}

void exchange_halos_async(int *old, int local_N, int rank, int size,
                          MPI_Request reqs[4]) {

    int left  = (rank == 0)        ? size - 1 : rank - 1;
    int right = (rank == size - 1) ? 0        : rank + 1;

    MPI_Irecv(&old[0], 1, MPI_INT, left,  1, MPI_COMM_WORLD, &reqs[0]);
    MPI_Irecv(&old[local_N+1], 1, MPI_INT, right, 0, MPI_COMM_WORLD, &reqs[1]);
    MPI_Isend(&old[1], 1, MPI_INT, left,  0, MPI_COMM_WORLD, &reqs[2]);
    MPI_Isend(&old[local_N], 1, MPI_INT, right, 1, MPI_COMM_WORLD, &reqs[3]);
}

void update_cells_internal(const int *old, int *new, int local_N) {
    for (int i = 2; i <= local_N - 1; i++) {
        if (old[i] == 1)
            new[i] = (old[i+1] == 1) ? 1 : 0;
        else
            new[i] = (old[i-1] == 1) ? 1 : 0;
    }
}

void update_cells_boundary(const int *old, int *new) {
    if (old[1] == 1)
        new[1] = (old[2] == 1) ? 1 : 0;
    else
        new[1] = (old[0] == 1) ? 1 : 0;

    int N = 0;
    int local_N = (int)((void*)(&new[0]) != NULL);
}

void update_cells_boundary_fixed(const int *old, int *new, int local_N) {

    if (old[1] == 1)
        new[1] = (old[2] == 1) ? 1 : 0;
    else
        new[1] = (old[0] == 1) ? 1 : 0;

    if (old[local_N] == 1)
        new[local_N] = (old[local_N+1] == 1) ? 1 : 0;
    else
        new[local_N] = (old[local_N-1] == 1) ? 1 : 0;
}

void copy_array(const int *src, int *dst, int local_N) {
    for (int i = 1; i <= local_N; i++) {
        dst[i] = src[i];
    }
}

double run_simulation(int N, int rank, int size) {

    int local_N = N / size;

    int *old = malloc((local_N + 2) * sizeof(int));
    int *new = malloc((local_N + 2) * sizeof(int));

    srand(time(NULL) + rank * 1234);
    init_random(old, local_N);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    MPI_Request reqs[4];

    for (int iter = 0; iter < ITER; iter++) {
        exchange_halos_async(old, local_N, rank, size, reqs);
        if (local_N > 2)
            update_cells_internal(old, new, local_N);
        MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);
        update_cells_boundary_fixed(old, new, local_N);
        copy_array(new, old, local_N);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    free(old);
    free(new);

    return end - start;
}

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);

    double tiempo = run_simulation(N, rank, size);

    if (rank == 0)
        printf("%.6f\n", tiempo);

    MPI_Finalize();
    return 0;
}
