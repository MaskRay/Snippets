/*
 * Palindromic tree
 */
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <type_traits>
using namespace std;

#define FOR(i, a, b) for (remove_cv<remove_reference<decltype(b)>::type>::type i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

const long N = 10000, AB = 127;
char a[N+1];
struct PalindromicTree { long len, sl, c[AB]; } pt[N+2];

void palindromic_tree(long n)
{
  long allo = 2, u = 1;
  pt[0].len = 0; pt[1].len = -1;
  pt[0].sl = pt[1].sl = 1;
  fill_n(pt[0].c, AB, 0);
  fill_n(pt[1].c, AB, 0);
  REP(i, n) {
    while (i-pt[u].len-1 < 0 || a[i-pt[u].len-1] != a[i])
      u = pt[u].sl;
    long c = a[i];
    if (! pt[u].c[c]) {
      long v = allo++, w = pt[u].sl;
      pt[v].len = pt[u].len+2;
      while (a[i-pt[w].len-1] != a[i])
        w = pt[w].sl;
      pt[v].sl = pt[w].c[c];
      fill_n(pt[v].c, AB, 0);
      pt[u].c[c] = v;
    }
    u = pt[u].c[c];
  }
}

void dump(int d, long x)
{
  printf("%ld %ld\n", x, pt[x].sl);
  REP(c, AB)
    if (pt[x].c[c] > 0) {
      printf("%*s%c ", 2*d+2, "", int(c));
      dump(d+1, pt[x].c[c]);
    }
}

int main()
{
  while (cin >> a) {
    palindromic_tree(strlen(a));
    puts("--- even");
    dump(0, 0);
    puts("--- odd");
    dump(0, 1);
  }
}
