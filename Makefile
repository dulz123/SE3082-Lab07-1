MPICC = mpicc
CFLAGS = -O2 -Wall -Wextra -std=c11
MPIRUN = mpirun
MPI_FLAGS ?=
NP ?= 4
PROGRAMS = sum_bcast sum_scatter sum_gather sum_reduce sum_allreduce sum_scan
.PHONY: all run benchmark clean
all: $(PROGRAMS)
sum_bcast: Exercise01/sum_bcast.c
	$(MPICC) $(CFLAGS) $< -o $@
sum_scatter: Exercise02/sum_scatter.c
	$(MPICC) $(CFLAGS) $< -o $@
sum_gather: Exercise03/sum_gather.c
	$(MPICC) $(CFLAGS) $< -o $@
sum_reduce: Exercise04/sum_reduce.c
	$(MPICC) $(CFLAGS) $< -o $@
sum_allreduce: Exercise05/sum_allreduce.c
	$(MPICC) $(CFLAGS) $< -o $@
sum_scan: Exercise06/sum_scan.c
	$(MPICC) $(CFLAGS) $< -o $@

run: all
	@set -e; for p in $(PROGRAMS); do $(MPIRUN) $(MPI_FLAGS) -np $(NP) ./$$p; done
benchmark: all
	python3 benchmark.py --mpirun "$(MPIRUN)" --flags="$(MPI_FLAGS)"
clean:
	rm -f $(PROGRAMS)
