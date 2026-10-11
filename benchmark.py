"""Run actual 2/4/8-process measurements; never substitute estimated times."""
import argparse, csv, shlex, statistics, subprocess
from pathlib import Path
p = argparse.ArgumentParser()
p.add_argument('--mpirun', default='mpirun')
p.add_argument('--flags', default='')
p.add_argument('--repeats', type=int, default=3)
a = p.parse_args()
if a.repeats < 1: p.error('--repeats must be positive')
root = Path(__file__).resolve().parent
results = root / 'Exercise07'
results.mkdir(exist_ok=True)
rows = []
with (results / 'benchmark_output.txt').open('w') as log:
    for np in (2, 4, 8):
        for name in ('bcast', 'scatter', 'gather', 'reduce', 'allreduce', 'scan'):
            times = []
            for repeat in range(a.repeats):
                cmd = shlex.split(a.mpirun) + shlex.split(a.flags) + ['-np', str(np), './sum_' + name]
                run = subprocess.run(cmd, cwd=root, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                log.write(' '.join(cmd) + '\n' + run.stdout + '\n'); log.flush()
                if run.returncode: raise SystemExit(run.stdout)
                timing = [line for line in run.stdout.splitlines() if line.startswith('TIME,')]
                if len(timing) != 1 or 'Correct? YES' not in run.stdout:
                    raise SystemExit('Missing timing or verification: ' + run.stdout)
                times.append(float(timing[0].split(',')[3]))
            rows.append([name, np, *times, statistics.median(times)])
            print(name, np, 'processes: median', rows[-1][-1], 'seconds', flush=True)
with (results / 'timings.csv').open('w', newline='') as f:
    w = csv.writer(f)
    w.writerow(['program', 'processes'] + ['run_' + str(i+1) for i in range(a.repeats)] + ['median_seconds'])
    w.writerows(rows)
lines = ['# Measured timing results', '', 'Medians of ' + str(a.repeats) + ' runs. Times are seconds.', '', '| Program | 2 processes | 4 processes | 8 processes |', '|---|---:|---:|---:|']
for name in ('bcast', 'scatter', 'gather', 'reduce', 'allreduce', 'scan'):
    lines.append('| ' + name + ' | ' + ' | '.join(f'{r[-1]:.9f}' for r in rows if r[0] == name) + ' |')
lines += ['', '## Fastest measured approach', '']
for np in (2, 4, 8):
    best = min((r for r in rows if r[1] == np), key=lambda r: r[-1])
    lines.append(f'- {np} processes: {best[0]} ({best[-1]:.9f} seconds).')
lines += ['', 'Scatter reduces the data sent to each process. Reduce combines values inside MPI, avoiding the manual root loop. Small-message overhead, scheduling, and hardware can change which version wins. Scan computes prefixes, so it performs a different result-distribution task. These short timings are noisy; repeat runs before drawing strong conclusions. Oversubscribed runs include CPU contention.']
(results / 'timing_results.md').write_text('\n'.join(lines) + '\n')
print('Saved Exercise07/timings.csv, timing_results.md, and benchmark_output.txt')
