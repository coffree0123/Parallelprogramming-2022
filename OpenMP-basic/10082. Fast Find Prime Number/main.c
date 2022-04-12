#include <stdio.h>
#define MAX_N 5000000
#define MAX_P 5050

void build_prime_table(int prime_table[], int is_prime[])
{
    int prime_idx = 0;
    // Find all possible prime
    for (int i = 2; i < MAX_N; i++)
    {
        if (is_prime[i] && prime_idx < MAX_P)
        {
            prime_table[prime_idx++] = i;
            // Clear all multiples of i
            for (int mul = 2; i * mul < MAX_N; mul++)
            {
                is_prime[i * mul] = 0;
            }
        }
    }
}

int find_prime(int L, int R, int prime_table)
{
    int count = 0;
}

int main()
{
    // First build a prime table
    int is_prime[MAX_N];
    int prime_table[MAX_P];
    memset(is_prime, 1, MAX_N);
    build_prime_table(prime_table, is_prime);

    int L, R;
    while (scanf("%d %d", &L, &R) != EOF)
    {
        printf("%d\n", find_prime(L, R, prime_table));
    }

    return 0;
}