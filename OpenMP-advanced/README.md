# OpenMP-advanced

本章目的在探討各種影響openmp performance的原因以及改善方式

## Private variable

Concept: Local variable is much easier to access than global ones

## Thread creation / Destroying

Concept: Thread create 和 destroy很花時間，所以可以讓omp parallel和omp for分開，不像omp parallel for 每次for完都要destroy下一輪重新create

E.g. Find prime number

## nowait

可以使用nowait 讓omp for的每個thread結束後不等其他thread就繼續走下去

## Critical region

Concept: critical section越多會越慢，如果可以的話盡量少用critical section，使用reduction的話效率會最高

## Double buffer

 使用兩個array並且交替作為對方的buffer，可以讓整份程式更容易平行化

E.g. Game of life