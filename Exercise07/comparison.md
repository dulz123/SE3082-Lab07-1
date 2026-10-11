# Exercise 7 — Comparison and thinking question

| Program | Main communication | Array allocation | Manual total loop on root? | Result location |
|---|---|---|---|---|
| Bcast | Bcast + Send/Recv | Full array on every rank | Yes | Root |
| Scatter | Scatter + Send/Recv | Full array on root; chunk on every rank | Yes | Root |
| Gather | Scatter + Gather | Full array on root; chunk on every rank; gathered sums on root | Yes | Root |
| Reduce | Scatter + Reduce | Full array on root; chunk on every rank | No | Root |
| Allreduce | Scatter + Allreduce | Full array on root; chunk on every rank | No | Every rank |
| Scan | Scatter + Scan | Full array on root; chunk on every rank | No | Prefix on each rank; global total on last rank |

Every program also uses an untimed Barrier before timing and an untimed Allreduce of the failure flag afterwards. These support timing and exit status, not the array-sum strategy.

## Timings
Run `make benchmark`. It runs every program three times with 2, 4, and 8 processes and creates `timings.csv`, `timing_results.md`, and `benchmark_output.txt` in this folder. The generated report names the measured fastest approach. No timing values have been invented or prefilled.

Timing starts after a barrier and excludes initialization, allocation and output. It ends after the sum communication at rank 0 (last rank for Scan). This is result-availability latency on the reporting rank, not maximum completion time across all ranks. Scan returns prefixes rather than an identical result distribution. Implementation and hardware determine performance; Reduce need not always win.

## When to use Scan instead of Allreduce
Use Scan when each process needs a cumulative result up to its own rank. For example, processes independently create output records. A scan over their record counts gives cumulative counts; subtracting the local count gives each process its starting offset in one shared output array. Counts 3, 5, 2, 4 produce prefixes 3, 8, 10, 14 and offsets 0, 3, 8, 10. Allreduce would return 14 everywhere, which does not provide each process's offset.

For this lab, add `sum_before_me` to each local running sum to obtain globally correct running sums.
