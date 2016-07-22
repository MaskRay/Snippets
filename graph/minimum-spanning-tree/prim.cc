/*
 * Prim's algorithm with binary heap to find a minimum spanning tree
 *
 * Time complexity: O(m*log(n))
 *
 * Verified by poj.org 1287
 */
#include <algorithm>
#include <climits>
#include <functional>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

#define FOR(i, a, b) for (long i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

typedef pair<long, long> pll;
const long N = 50, M = N*N;
long d[N];
vector<pll> adj[N];

long prim(long n, long m)
{
  long r = 0;
  priority_queue<pll, vector<pll>, greater<pll> > pq;
  fill_n(d, n, LONG_MAX);
  d[0] = 0;
  pq.push(make_pair(0L, 0L));
  while (pq.size()) {
    long w = pq.top().first, u = pq.top().second;
    pq.pop();
    if (d[u] < w) continue;
    d[u] = LONG_MIN;
    r += w;
    for (vector<pll>::iterator it = adj[u].begin(); it != adj[u].end(); ++it)
      if (it->second < d[it->first]) {
        d[it->first] = it->second;
        pq.push(make_pair(d[it->first], it->first));
      }
  }
  return r;
}

int main()
{
  long n, m, u, v, w;
  while (cin >> n >> m) {
    REP(i, n)
      adj[i].clear();
    REP(i, m) {
      cin >> u >> v >> w;
      u--, v--;
      adj[u].push_back(make_pair(v, w));
      adj[v].push_back(make_pair(u, w));
    }
    cout << prim(n, m) << endl;
  }
}
