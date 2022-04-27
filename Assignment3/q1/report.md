# Q1

- We can run with a maximum n of 2048.
- We get the best performance in this case with number of threads being 4096
- Here is a table for varying threads

| Threads | Time    |
|---------|---------|
| 1024    | 4469739 |
| 2048    | 902525  |
| 4096    | 6850    |
| 8192    | 7834    |
| 16384   | 7010    |

- We see that using tree optimization does not help much with performance, this might be due to grid_sync that we use.
- The best omp program takes around 2404623 microseconds with n = 2098 and 16 threads.

# q2

- The maxmimum n we run it for is 8192.
- We get the best performance from choosing 8192 threads.
- here is a table for the results

| Threads | Time    |
|---------|---------|
| 256   | 200673 |
| 512    | 94093  |
| 1024    | 60217    |
| 2048    | 26911    |
| 4096   | 17200    |
| 8192   | 8698    |

- for the same value, without using Shared memory, we get a time of 1694448 microseconds, which is a lot slower.
- The best performing OMP program timed around 292212 microsenconds, with the same n and 16 CPU threads.