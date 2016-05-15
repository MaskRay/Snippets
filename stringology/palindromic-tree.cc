/*
 * Palindromic tree
 *
 * Input
 * abababbababacadda
 */
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <type_traits>
using namespace std;

#define FOR(i, a, b) for (remove_cv<remove_reference<decltype(b)>::type>::type i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)
#define ROF(i, a, b) for (remove_cv<remove_reference<decltype(b)>::type>::type i = (b); --i >= (a); )

const int N = 1000, AB = 26;

char s[N+1];
int allo;
struct Node {
  int sl, len, c[26];
} a[N+2];

int new_node()
{
  fill_n(a[allo].c, AB, 0);
  return allo++;
}

int get_suff(int i, int x)
{
  while (i-1-a[x].len < 0 || s[i-1-a[x].len] != s[i])
    x = a[x].sl;
  return x;
}

void build()
{
  allo = 0;
  new_node();
  a[0].sl = 1; a[0].len = 0;
  new_node();
  a[1].sl = 1; a[1].len = -1;
  int x = 1;
  for (int i = 0; s[i]; i++) {
    x = get_suff(i, x);
    int& v = a[x].c[s[i]-'a'];
    if (! v) {
      int y = new_node();
      a[y].len = a[x].len+2;
      a[y].sl = a[get_suff(i, a[x].sl)].c[s[i]-'a'];
      v = y;
    }
    x = v;
  }
}

void dump(int d, int x)
{
  printf("%d\n", x);
  REP(c, AB)
    if (a[x].c[c] > 0) {
      printf("%*s%c ", 2*d+2, "", 'a'+c);
      dump(d+1, a[x].c[c]);
    }
}

int main()
{
  while (cin >> s) {
    build();
    dump(0, 0);
    dump(0, 1);
  }
}
