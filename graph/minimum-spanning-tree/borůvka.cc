/*
 * Borůvka's algorithm to find a minimum spanning tree
 *
 * Time complexity: O(m*log(n))
 *
 * Verified by poj.org 1287
 */
#include <algorithm>
#include <iostream>
using namespace std;

#define FOR(i, a, b) for (long i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

const long N = 50, M = N*N;
long R[N], B[N], S[N];
struct Edge { long u, v, w; } es[M], *p[N];

long boruvka(long n, long m)
{
  long r = 0;
  REP(i, n)
    B[i] = i;
  fill_n(R, n, -1);
  fill_n(S, n, 1);
  for (long c = n; c > 1; ) {
    fill_n(p, n, (Edge*)0);
    for (Edge* it = es; it < es+m; it++)
      if (B[it->u] != B[it->v]) {
        if (! p[B[it->u]] || it->w < p[B[it->u]]->w)
          p[B[it->u]] = it;
        if (! p[B[it->v]] || it->w < p[B[it->v]]->w)
          p[B[it->v]] = it;
      }
    REP(i, n)
      if (p[i]) {
        long bu = B[p[i]->u], bv = B[p[i]->v];
        if (bu != bv) {
          c--;
          r += p[i]->w;
          if (S[bu] < S[bv])
            swap(bu, bv);
          long last;
          for (long v = bv; v >= 0; v = R[v]) {
            last = v;
            B[v] = bu;
          }
          R[last] = R[bu];
          R[bu] = bv;
          S[bu] += S[bv];
        }
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
    cout << boruvka(n, m) << endl;
  }
}
