#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define ITER 1000

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) printf("Uso: mpirun -np <p> ./prog N\n");
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);

    // División uniforme
    int local_N = N / size;
    int start = rank * local_N + 1;

    // Reservar con ghosts
    int *old = malloc((local_N + 2) * sizeof(int));
    int *new = malloc((local_N + 2) * sizeof(int));

    // Inicialización consistente con secuencial
    srand(time(NULL) + rank);
    for (int i = 1; i <= local_N; i++)
        old[i] = rand() % 2;

    MPI_Barrier(MPI_COMM_WORLD);
    double start_t = MPI_Wtime();

    for (int iter = 0; iter < ITER; iter++) {

        int left  = (rank == 0) ? size - 1 : rank - 1;
        int right = (rank == size - 1) ? 0 : rank + 1;

        MPI_Request reqs[4];

        // Recibir primero (no bloqueante)
        MPI_Irecv(&old[0],      1, MPI_INT, left,  0, MPI_COMM_WORLD, &reqs[0]);
        MPI_Irecv(&old[local_N+1], 1, MPI_INT, right, 1, MPI_COMM_WORLD, &reqs[1]);

        // Enviar después (no bloqueante)
        MPI_Isend(&old[1],         1, MPI_INT, left,  1, MPI_COMM_WORLD, &reqs[2]);
        MPI_Isend(&old[local_N],   1, MPI_INT, right, 0, MPI_COMM_WORLD, &reqs[3]);

        // Esperar todas las comm
        MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);

        // Aplicar reglas (idéntico a tu código)
        for (int i = 1; i <= local_N; i++) {

            if (old[i] == 1) {
                new[i] = (old[i+1] == 1) ? 1 : 0;
            } else {
                new[i] = (old[i-1] == 1) ? 1 : 0;
            }
        }

        // Copiar
        for (int i = 1; i <= local_N; i++)
            old[i] = new[i];
    }

    double end_t = MPI_Wtime();

    if (rank == 0)
        printf("Tiempo: %.6f\n", end_t - start_t);

    free(old);
    free(new);

    MPI_Finalize();
    return 0;
}
