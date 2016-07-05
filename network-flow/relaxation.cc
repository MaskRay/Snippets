/*
 * Solve the minimum-cost flow problem by relaxation algorithm
 * Ref: Network Flows: Theory, Algorithm and Applications 9.10 Relaxation algorithm
 *
 * The first line contains $n, m$
 * The next $n$ lines specifies supply/demand of each vertex
 * The next $m$ lines $u v w c$ specifies an arc from $u$ to $v$ with cost $w$ and capacity $c$
 *
 * Print the cost if feasible, otherwise LONG_MAX.
 */
#include <cassert>
#include <algorithm>
#include <climits>
#include <iostream>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>
using namespace std;

typedef pair<long, long> pll;
#define FOR(i, a, b) for (remove_cv<remove_reference<decltype(b)>::type>::type i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

const long N = 10000, M = 60000;
bool in[N], vis[N];
long ex[N], h[N], q[N];
set<long> exs;
struct Edge { long v, c, w; Edge *next, *dual; } *e[N], pool[M << 1], *pre[N], *allo;

void insert(long u, long v, long c, long w)
{
  allo->v = v; allo->c = c; allo->w = w; allo->next = e[u]; e[u] = allo++;
  allo->v = u; allo->c = 0; allo->w = - w; allo->next = e[v]; e[v] = allo++;
  e[u]->dual = e[v];
  e[v]->dual = e[u];
}

long adjust_potential(long n, long ns)
{
  long d = LONG_MAX;
  REP(i, ns) {
    long u = q[i];
    for (Edge* it = e[u]; it; it = it->next)
      if (! vis[it->v] && it->c > 0) {
        long rw = it->w-h[u]+h[it->v];
        if (! rw) {
          if ((ex[u] -= it->c) <= 0)
            exs.erase(u);
          if ((ex[it->v] += it->c) > 0)
            exs.insert(it->v);
          it->dual->c += it->c;
          it->c = 0;
        } else
          d = min(d, rw);
      }
  }
  if (d < LONG_MAX)
    REP(u, n)
      if (vis[u])
        h[u] += d;
  return d;
}

long relaxation(long n)
{
  long cost = 0;
  fill_n(h, n, 0);
  fill_n(in, n, false);
  fill_n(vis, n, false);
  exs.clear();
  REP(i, n)
    if (ex[i] > 0) {
      in[i] = true;
      exs.insert(i);
    }
  while (exs.size()) {
    long s = *exs.begin(), *fore = q, *rear = q, excess = ex[s], r = 0;
    assert(ex[s] > 0);
    vis[s] = true;
    *rear++ = s;
    for (Edge* it = e[s]; it; it = it->next)
      if (it->w == h[s]-h[it->v])
        r += it->c;
    while (excess <= r) {
      long u = *fore++, v;
      for (Edge* it = e[u]; it; it = it->next) {
        v = it->v;
        if (it->c > 0 && ! vis[v] && it->w == h[u]-h[v]) {
          vis[v] = true;
          pre[v] = it;
          *rear++ = v;
          if (ex[v] < 0) {
            long d = min(ex[s], - ex[v]);
            for (u = v; u != s; u = pre[u]->dual->v)
              d = min(d, pre[u]->c);
            if ((ex[s] -= d) <= 0)
              exs.erase(s);
            if ((ex[v] += d) > 0)
              exs.insert(v);
            for (u = v; u != s; u = pre[u]->dual->v) {
              pre[u]->c -= d;
              pre[u]->dual->c += d;
            }
            goto flow;
          }
          excess += ex[v];
          for (Edge* it2 = e[v]; it2; it2 = it2->next) {
            if (it2->c > 0 && ! vis[it2->v] && it2->w == h[v]-h[it2->v])
              r += it2->c;
            if (it2->dual->c > 0 && vis[it2->v] && it2->w == h[v]-h[it2->v])
              r -= it2->dual->c;
          }
          if (excess > r) goto potential;
        }
      }
    }
potential:
    {
      long d = adjust_potential(n, rear-q);
      if (d == LONG_MAX) return LONG_MAX;
      cost += (excess-r)*d;
    }
flow:;
     REP(i, rear-q)
       vis[q[i]] = false;
  }
  return cost;
}

int main()
{
  ios_base::sync_with_stdio(0);
  long cases, n, m, u, v, w, c, cost = 0;
  allo = pool;
  cin >> n >> m;
  REP(i, n)
    cin >> ex[i];
  REP(i, m) {
    cin >> u >> v >> w >> c;
    // arc reversal transformation converts negative arc costs to positive
    if (w < 0) {
      ex[u] -= c;
      ex[v] += c;
      cost += w*c;
      swap(u, v);
      w = - w;
    }
    insert(u, v, c, w);
  }

  cout << cost+relaxation(n) << '\n';
}
