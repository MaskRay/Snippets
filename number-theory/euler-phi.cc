/*
 * Calculating Euler's phi by the sieve of Erastosthenes
 */
#include <algorithm>
#include <cstdio>
#include <numeric>
using namespace std;

const long N = 30;

long euler[N];

void eratosthenes(int n)
{
  iota(euler, euler+n, 0L);
  for (long i = 2; i < n; i++)
    if (euler[i] == i)
      for (long j = i; j < n; j += i)
        euler[j] = euler[j] / i * (i-1);
}

int main()
{
  eratosthenes(N);
  for (long i = 1; i < N; i++)
    printf("%ld %ld\n", i, euler[i]);
}
