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
    if (rank == 0) array = checked_malloc(N * sizeof(int));
    int *local_chunk = checked_malloc(chunk_size * sizeof(int));
    if (rank == 0)
        for (int i = 0; i < N; ++i) array[i] = i + 1;

    /* Exclude allocation, initialization and printing from timing. */
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();
    MPI_Scatter(array, chunk_size, MPI_INT, local_chunk, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    long long local_sum = 0;
    for (int i = 0; i < chunk_size; ++i) local_sum += local_chunk[i];
    long long prefix_sum = 0;
    MPI_Scan(&local_sum, &prefix_sum, 1, MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);
    double elapsed = MPI_Wtime() - start;
    long long expected = (long long)N * (N + 1) / 2;
    int failed = 0;
    long long k = (long long)(rank + 1) * chunk_size;
    failed = prefix_sum != k * (k + 1) / 2;
    printf("Rank %d: local_sum = %lld | prefix_sum = %lld | sum_before_me = %lld | Prefix correct? %s\n",
           rank, local_sum, prefix_sum, prefix_sum - local_sum, failed ? "NO" : "YES");
    if (rank == size - 1) {
        failed |= prefix_sum != expected;
        printf("[scan] Total sum = %lld | Expected = %lld | Correct? %s\n", prefix_sum, expected, failed ? "NO" : "YES");
        printf("TIME,scan,%d,%.9f\n", size, elapsed);
    }
    /* Separate untimed validation so failures return a nonzero exit code. */
    int any_failed;
    MPI_Allreduce(&failed, &any_failed, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    free(local_chunk);
    free(array);
    MPI_Finalize();
    return any_failed ? 1 : 0;
}
