# Lab 7 — next steps

1. Extract this ZIP in Windows (right-click > Extract All). Open the extracted Lab07_Completed folder and copy its location from the address bar.
2. Open Ubuntu. Change directory to that folder. For example, Windows `C:\Users\Dulan\Downloads\Lab07_Completed` becomes `/mnt/c/Users/Dulan/Downloads/Lab07_Completed`. Use your actual path:

   `cd "/mnt/c/Users/Dulan/Downloads/Lab07_Completed"`

3. If MPI is not installed, run each command:

   `sudo apt update`

   `sudo apt install build-essential openmpi-bin libopenmpi-dev python3`

4. Compile: `make`
5. Run all six programs: `make run`
6. Every program should show `Correct? YES` and total `500000500000`. Scan also checks every prefix. Rank lines may appear in a different order; that is normal.
7. Measure timings: `make benchmark`
8. If Open MPI says there are not enough slots, use `make run MPI_FLAGS=--oversubscribe` and `make benchmark MPI_FLAGS=--oversubscribe`. This permits more MPI processes than available CPU slots but timings include contention.
9. Read `Exercise07/comparison.md` and generated `Exercise07/timing_results.md`. Understand the code and adjust explanations to your understanding.
10. If your lecturer separately requires screenshots, take your own Ubuntu output screenshots. This lab README does not explicitly require screenshots.
11. Run `make clean` to remove executables. Copy the extracted folder contents into your Lab 7 GitHub repository folder. Do not replace its `.git` folder. Include the source folders, Makefile, benchmark script, comparison notes and your measured results. In GitHub Desktop, review changes, enter `Complete Lab 7 MPI collective exercises`, click Commit, then Push origin. Do not submit only the ZIP in place of source files.
12. Open the repository website and confirm your `.c` files and Makefile are visible. Submit the repository URL wherever your lecturer requests it. The provided Lab 7 instructions do not specify collaborator details.

## Package status
All six source programs, Makefile, benchmark automation and comparison answer are included. MPI was unavailable in the creation environment and package installation was blocked, so actual MPI compilation and execution must be checked on your Ubuntu installation. Timing results are generated only after real execution. Original lab instructions are preserved in Readme.md.
