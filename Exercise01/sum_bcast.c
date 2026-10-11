#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define N 1000000

/* Abort the whole MPI job if one allocation fails. */
static void *checked_malloc(size_t bytes) {
    void *p = malloc(bytes);
    if (!p) { fprintf(stderr, "Memory allocation failed\n"); MPI_Abort(MPI_COMM_WORLD, 1); exit(1); }
    return p;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (N % size != 0) {
        if (rank == 0) fprintf(stderr, "Process count must evenly divide %d\n", N);
        MPI_Finalize();
        return 1;
    }
    int chunk_size = N / size;
    int *array = NULL;
    array = checked_malloc(N * sizeof(int));
    if (rank == 0)
        for (int i = 0; i < N; ++i) array[i] = i + 1;

    /* Exclude allocation, initialization and printing from timing. */
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();
    MPI_Bcast(array, N, MPI_INT, 0, MPI_COMM_WORLD);
    long long local_sum = 0;
    for (int i = rank * chunk_size; i < (rank + 1) * chunk_size; ++i) local_sum += array[i];
    long long total_sum = local_sum;
    if (rank != 0) {
        MPI_Send(&local_sum, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    } else {
        for (int r = 1; r < size; ++r) {
            long long received;
            MPI_Recv(&received, 1, MPI_LONG_LONG, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_sum += received;
        }
    }
    double elapsed = MPI_Wtime() - start;
    long long expected = (long long)N * (N + 1) / 2;
    int failed = 0;
    printf("Rank %d: local_sum = %lld\n", rank, local_sum);
    if (rank == 0) {
        failed = total_sum != expected;
        printf("[bcast] Total sum = %lld | Expected = %lld | Correct? %s\n", total_sum, expected, failed ? "NO" : "YES");
        printf("TIME,bcast,%d,%.9f\n", size, elapsed);
    }
    /* Separate untimed validation so failures return a nonzero exit code. */
    int any_failed;
    MPI_Allreduce(&failed, &any_failed, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    free(array);
    MPI_Finalize();
    return any_failed ? 1 : 0;
}
