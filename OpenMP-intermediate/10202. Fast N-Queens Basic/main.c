/* begin */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAXN 20
int n; /* a global n */
int matrix[MAXN][MAXN];

/* ok */
int ok(int position[], int next, int test)
{
    if (matrix[next][test] == 1)
        return 0;
    for (int i = 0; i < next; i++)
        if (position[i] == test ||
            (abs(test - position[i]) == next - i))
            return 0;
    return 1;
}
/* queen */
int queen(int position[], int next)
{
    if (next >= n)
        return 1;
    int sum = 0;
    for (int test = 0; test < n; test++)
        if (ok(position, next, test))
        {
            position[next] = test;
            sum += queen(position, next + 1);
        }
    return sum;
}

void print_matrix(int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d%s", matrix[i][j], (j == n - 1) ? "\n" : " ");
        }
    }
}

int main()
{
    int cases = 1;
    char s[32] = {};
    while (scanf("%d", &n) == 1)
    {
        for (int i = 0; i < n; i++)
        {
            scanf("%s", s);
            for (int j = 0; j < n; j++)
            {
                if (s[j] == '*')
                    matrix[i][j] = 1;
            }
        }

#ifdef DEBUG
        print_matrix(n);
#endif
        int numSolution = 0;

#pragma omp parallel for reduction(+ \
                                   : numSolution) collapse(3) schedule(dynamic, 1)
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                for (int k = 0; k < n; k++)
                {
                    if (i == j || abs(j - i) == 1 || i == k || abs(i - k) == 2 ||
                         j == k || abs(j - k) == 1 || matrix[1][j] == 1 || matrix[0][i] == 1 || matrix[2][k] == 1)
                        continue;
                    int position[MAXN];
                    position[0] = i;
                    position[1] = j;
                    position[2] = k;
                    int num = queen(position, 3);
                    numSolution += num;
                }
            }
        }
        printf("Case %d: %d\n", cases, numSolution);
        cases += 1;
        memset(matrix, 0, sizeof(matrix));
    }
    return 0;
}