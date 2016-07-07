/*
 * Ukkonen's suffix tree of multiple strings
 *
 * Adapted from Oleksandr Kulkov http://codeforces.com/blog/entry/16780
 * See https://stackoverflow.com/questions/9452701/ukkonens-suffix-tree-algorithm-in-plain-english for explanation
 *
 * The first line contains an integer `m` : the number of strings
 * Each of the next `m` lines contains a string
 *
 * Input
 * 3
 * abbabb
 * ababbba
 * bbabb
 *
 */
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <vector>
using namespace std;

#define FOR(i, a, b) for (remove_cv<remove_reference<decltype(b)>::type>::type i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

const long N = 1000, V = 2*N;
long allo, fork, suff, len[V], sl[V];
const char* pos[N];
map<int, int> to[V];

long new_node(const char* p, long l)
{
  pos[allo] = p;
  len[allo] = l;
  sl[allo] = 0;
  to[allo].clear();
  return allo++;
};

void init()
{
  allo = fork = suff = 0;
  new_node(0, 0);
}

void add(const char* s)
{
  long size = strlen(s);
  REP(i, size+1) {
    long last = 0, c = s[i]; // may be NUL
    suff++;
    while (suff > 0) {
      long cc = s[i+1-suff]; // may be NUL
      auto it = to[fork].find(cc);
      while (it != to[fork].end() && suff > len[it->second] && to[it->second].size()) {
        fork = it->second;
        suff -= len[fork];
        cc = s[i+1-suff]; // may be NUL
        it = to[fork].find(cc);
      }
      if (it == to[fork].end()) {
        if (c)
          to[fork][cc] = new_node(&s[i+1-suff], size-(i+1-suff));
        sl[last] = fork;
        last = 0;
      } else {
        long u, t = pos[it->second][suff-1]; // may be NUL
        if (c == t) {
          sl[last] = fork;
          break;
        }
        if (! t) { // extend leaf of a preceding string
          u = it->second;
          to[u][c] = new_node(&s[i], size-i);
        } else {
          u = new_node(pos[it->second], suff-1);
          if (c)
            to[u][c] = new_node(&s[i], size-i);
          to[u][t] = it->second;
          pos[it->second] += suff-1;
          len[it->second] -= suff-1;
          it->second = u;
        }
        sl[last] = u;
        last = u;
      }
      if (fork)
        fork = sl[fork];
      else
        suff--;
    }
  }
}

void dump(long d, long x)
{
  printf("%ld %ld\n", x, sl[x]);
  for (auto &c: to[x]) {
    int v = c.second;
    printf("%*s%s ", int(2*d+2), "", string(pos[v]).substr(0, len[v]).c_str());
    dump(d+1, v);
  }
}

int main()
{
  long m;
  while (cin >> m) {
    vector<string> s(m);
    init();
    for (auto& x : s) {
      cin >> x;
      add(x.c_str());
    }
    dump(0, 0);
  }
}
