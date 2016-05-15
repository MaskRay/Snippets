/*
 * Duval's Lyndon factorization algorithm
 *
 * Input
 * abcabcabbdababecec
 */
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

const long N = 1000;
char a[N+1];

void duval(int n)
{
  for (int s = 0; s < n; ) {
    int i = s, j = s+1;
    while (j < n && a[i] <= a[j]) {
      // invariant
      // $j-s = |l^n p|, j-i = |l|$, $p$ is a proper prefix of $l$
      if (a[i] < a[j])
        i = s;
      else
        i++;
      j++;
    }
    printf("%s %d\n", string(a+s, j-i).c_str(), (j-s)/(j-i));
    s += (j-s)/(j-i)*(j-i);
  }
}

int main()
{
  while (cin >> a)
    duval(strlen(a));
}
