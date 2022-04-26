#include <omp.h>
#include <stdio.h>
 
int max(int a, int b) {
    return (a >= b)? a : b;
}
 
int min(int a, int b) {
    return (a >= b)? b : a;
}
 
int W[10005];
int V[10005];
int dp[2][1000005];
 
int main() {
    int N, M;
    scanf("%d %d", &N, &M);
 
    for (int i = 1; i <= N; i++) {
        scanf("%d %d", &W[i], &V[i]);
    }
 
#pragma omp parallel
{
    for (int i = 1; i <= N; i++) {
#pragma omp for nowait
        for (int w = 1; w < W[i]; w++) {
            dp[i % 2][w] = dp[(i + 1) % 2][w];
        }

#pragma omp for
        for (int w = W[i]; w <= M; w++) {
            dp[i % 2][w] = max(dp[(i + 1) % 2][w - W[i]] + V[i], dp[(i + 1) % 2][w]);
        }
    }
}
 
    printf("%d\n", dp[N % 2][M]);
 
    return 0;
}