#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Uso: %s <tamano_carretera>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int ITER = 10000;  // Número de iteraciones (puedes modificarlo)

    int *old = malloc(N * sizeof(int));
    int *new = malloc(N * sizeof(int));

    if (!old || !new) {
        printf("Error al asignar memoria\n");
        return 1;
    }

    // Inicialización aleatoria de la carretera
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        old[i] = rand() % 2;   // 0 o 1
    }

    // Contabilidad
    int total_carros = 0;
    for (int i = 0; i < N; i++)
        if (old[i] == 1)
            total_carros++;

    if (total_carros == 0) {
        printf("No hay carros en la carretera, velocidad = 0\n");
        return 0;
    }

    long movimientos_totales = 0;

    // Medición de tiempo
    clock_t start = clock();

    // --- Bucle principal ---
    for (int t = 0; t < ITER; t++) {

        int movimientos_iter = 0;

        // Actualizamos todas las celdas
        for (int i = 0; i < N; i++) {

            int derecha = (i + 1) % N;

            // Regla de movimiento
            if (old[i] == 1 && old[derecha] == 0) {
                new[i] = 0;
                new[derecha] = 1;
                movimientos_iter++;
            } else if (!(old[derecha] == 1 && new[derecha] == 1)) {
                // Solo preservamos cuando no fue sobreescrito por un movimiento
                new[i] = old[i];
            }
        }

        movimientos_totales += movimientos_iter;

        // Intercambiamos old y new
        int *tmp = old;
        old = new;
        new = tmp;
    }

    clock_t end = clock();

    double tiempo = (double)(end - start) / CLOCKS_PER_SEC;

    // Velocidad promedio (0-1)
    double velocidad_promedio = (double) movimientos_totales / (double)(total_carros * ITER);

    printf("Tiempo de ejecucion: %f segundos\n", tiempo);
    printf("Velocidad promedio: %f\n", velocidad_promedio);

    free(old);
    free(new);
    return 0;
}
