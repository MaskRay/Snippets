/*
 * A variant of Knuth-Morris-Pratt algorithm to calculate the border function on each node of a trie
 *
 * The first line contains the number of strings $n$ and the following $n$ lines describe the strings
 *
 * Input
 * 2
 * ababab
 * ababaa
 */
#include <cstdio>
#include <iostream>
#include <map>
#include <string>
using namespace std;

const long N = 10000;
char a[N];
long pi[N], border[N];
map<char, long> ch[N];

void knuth_morris_pratt(long i, long x, long jj)
{
  for (auto& c: ch[x]) {
    a[i] = c.first;
    long j = jj;
    if (i)
      pi[i-1] = j >= 0 && a[i] == a[j] ? j ? pi[j-1] : -1 : j;
    while (j >= 0 && a[j] != c.first)
      j = j ? pi[j-1] : -1;
    border[c.second] = ++j;
    knuth_morris_pratt(i+1, c.second, j);
  }
}

void dump(long d, long x)
{
  for (auto& c: ch[x]) {
    printf("%*s%c %ld %ld\n", int(2*(d+1)), "", c.first, x, border[c.second]);
    dump(d+1, c.second);
  }
}

int main()
{
  long n, id = 1;
  string a;
  while (cin >> n) {
    while (n--) {
      cin >> a;
      long x = 0;
      for (char c: a) {
        if (! ch[x].count(c))
          ch[x][c] = id++;
        x = ch[x][c];
      }
    }
    knuth_morris_pratt(0, 0, -1);
    dump(0, 0);
  }
}
