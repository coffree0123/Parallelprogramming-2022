# OpenMP-intermediate

## Schedule Clause
指定OpenMp要如何分配thread
```c
schedule(policy, chunk)
```
- Policy:
    - **static**: Round-robin policy
    
    - **dynamic**: When a thread finishes it asks for more, and the chunk size is fixed

    - **guided**: When a thread finishes it asks for more, and the chunk size decreases exponentially, and will not be smaller than the chunk size

    - **runtime**: The policy is determined by an environment variable OMP SCHEDULE or calling a function omp set policy

    - **auto**: This policy selects a scheduling automatically

- runtime usage
    function:
    ```c
    void omp_set_schedule(omp_sched_t kind, int modifier);
    ```
    **kind**: scheduling policy
    **modifier**: chunk size
    - omp_sched_static (integer 1) for static
    - omp_sched_dynamic (integer 2) for dynamic
    - omp_sched_guided (integer 3) for guided
    - omp_sched_auto (integer 4) for auto

    example:
    ```c
    ...
    omp_set_schedule(policy, chunk);
    #pragma omp parallel for schedule(runtime)
    ...
    ```
## Measure Time
usage:
```c
double omp_get_wtime();
```

## Parallel sections
當我們需要讓不同thread執行不同段程式碼時就可以使用parallel sections
usage:
```c
#pragma omp parallel sections
{
#pragma omp section
    /* section 1 */
#pragma omp section
    /* section 2 */
}
```

