#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int N;

    // Inicializa o MPI
    MPI_Init(&argc, &argv);

    // Obtém o rank do processo
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Obtém o número total de processos
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Verifica argumento da linha de comando
    if (argc != 2) {
        if (rank == 0) {
            printf("Uso: mpirun -np <processos> ./media_mpi <N>\n");
        }

        MPI_Finalize();
        return 1;
    }

    // Tamanho do vetor local
    N = atoi(argv[1]);

    // Inicializa seed aleatória diferente para cada processo
    srand(time(NULL) + rank);

    // Aloca vetor local
    float *local_vector = (float *)malloc(N * sizeof(float));

    // Gera números aleatórios entre 0 e 1
    for (int i = 0; i < N; i++) {
        local_vector[i] = rand() / (float)RAND_MAX;
    }

    // Calcula soma local
    float local_sum = 0.0f;

    for (int i = 0; i < N; i++) {
        local_sum += local_vector[i];
    }

    // Calcula média local
    float local_avg = local_sum / N;

    // Imprime informações locais
    printf("[Processo %d] Soma local: %.3f, Média local: %.4f\n",
           rank, local_sum, local_avg);

    // Variável para soma global
    float global_sum = 0.0f;

    // Redução da soma global
    MPI_Reduce(
        &local_sum,
        &global_sum,
        1,
        MPI_FLOAT,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    // Processo 0 calcula e imprime média global
    if (rank == 0) {
        float global_avg = global_sum / (N * size);

        printf("\n[Soma global] %.3f\n", global_sum);
        printf("[Média global] %.4f\n", global_avg);
    }

    // Libera memória
    free(local_vector);

    // Finaliza MPI
    MPI_Finalize();

    return 0;
}
