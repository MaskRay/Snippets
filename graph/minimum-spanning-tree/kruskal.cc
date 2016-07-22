/*
 * Kruskal's algorithm to find a minimum spanning tree
 *
 * Time complexity: sorting + O(m+n*alpha(m, n))
 *
 * Verified by poj.org 1287
 */
#include <algorithm>
#include <iostream>
using namespace std;

#define FOR(i, a, b) for (long i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

const long N = 50, M = N*N;
long uf[N];
struct Edge {
  long u, v, w;
  bool operator<(const Edge& o) const {
    return w < o.w;
  }
} es[M];

long find(long x)
{
  while (uf[x] >= 0) {
    if (uf[uf[x]] >= 0)
      uf[x] = uf[uf[x]];
    x = uf[x];
  }
  return x;
}

long kruskal(long n, long m)
{
  long r = 0;
  fill_n(uf, n, -1);
  sort(es, es+m);
  for (Edge* it = es; it < es+m; it++) {
    long u = find(it->u), v = find(it->v);
    if (u != v) {
      r += it->w;
      if (uf[u] > uf[v])
        swap(u, v);
      uf[u] += uf[v];
      uf[v] = u;
    }
  }
  return r;
}

int main()
{
  long n, m;
  while (cin >> n >> m) {
    REP(i, m) {
      cin >> es[i].u >> es[i].v >> es[i].w;
      es[i].u--;
      es[i].v--;
    }
    cout << kruskal(n, m) << endl;
  }
}
