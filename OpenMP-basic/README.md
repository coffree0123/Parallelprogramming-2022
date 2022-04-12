# OpenMP-basic
[Toc]

## Introduction
**OpenMP(Open Multiprocessing)** is an API that supports multi-platform shared memory multiprocessing programming in C, C++, and Fortran, on most processor architectures.

1. 使用前置處理的方式運作(compiler preprocessor directive)
2. 可以進行所謂"incremental" parallelism

usage:
```c
#pragma omp parallel
statement
```
compile:
```bash
gcc -fopenmp hello.c -o hello-omp
```

## Useful function
Define in <omp.h>
1. omp_get_num_procs
2. omp_get_num_threads
3. omp_set_num_threads
4. omp_get_thread_num

## Loop
usage:
```c
#pragma omp parallel for
```
example:
```c
#pragma omp parallel for
    for (int i = 0; i < n; i++) {
        printf("thread %d runs index %d\n", omp_get_thread_num(), i);
    }
```

## Private
有時候我們希望在parallel for裡面的變數可以讓各自thread擁有自己的記憶體空間，這時候就需要private
usage:
```c
private(variable)
```
example:
```c
    ...
    int position[10];
#pragma omp parallel for private(position)
    for (int i = 0; i < 10; i++) {
        position[0] = i;
    }
    ...
```

## Reduction
我們有時候會需要把不同thread的計算結果累計到某個共用變數上，這時候就可以使用OpenMP提供的reduction機制
usage:
```c
reduction(operation : variable)
```
example:
```c
    ...
    int position[10];
    int num_sol = 0;
#pragma omp parallel for private(position) reduction(+ : num_sol)
    for (int i = 0; i < 10; i++) {
        position[0] = i;
        num_sol += i;
    }
    ...
```

## Firstprivate
如果我們想要讓private variable能夠有一個固定的初始值，我們可以使用firstprivate機制來產生private variable
usage:
```c
firstprivate(variable)
```

## Lastprivate
如果我們想讓thread中的private variable傳回global，可以使用lastprivate，他會將最後一個iteration的private variable的值傳回
usage:
```c
lastprivate(variable)
```

## Critical section
可以使用critical directive來讓openmp幫忙建立critical section
usage:
```c
#pragma omp critical
```
example:
```c
#pragma omp critical
    num_sol++;
```