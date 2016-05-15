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
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <vector>
using namespace std;

#define FOR(i, a, b) for (remove_cv<remove_reference<decltype(b)>::type>::type i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

const long N = 1000, V = 2*N;
int allo, fork, suff, len[V], sl[V];
const char* pos[N];
map<int, int> to[V];

int new_node(const char* p, int l)
{
  pos[allo] = p;
  len[allo] = l;
  to[allo].clear();
  return allo++;
};

void init()
{
  allo = fork = suff = 0;
  new_node(0, 0);
}

void add(const string& s)
{
  REP(i, s.size()+1) {
    int last = 0, c = i == s.size() ? -1 : s[i];
    suff++;
    while (suff > 0) {
      int cc = i+1-suff == s.size() ? -1 : s[i+1-suff];
      auto it = to[fork].find(cc);
      while (it != to[fork].end() && suff > len[it->second]) {
        fork = it->second;
        suff -= len[fork];
        cc = i+1-suff == s.size() ? -1 : s[i+1-suff];
        it = to[fork].find(cc);
      }
      if (it == to[fork].end()) {
        if (c >= 0)
          to[fork][cc] = new_node(&s[i+1-suff], s.size()-(i+1-suff));
        sl[last] = fork;
        last = 0;
      } else {
        int t = pos[it->second][suff-1];
        if (c == t) {
          sl[last] = fork;
          break;
        } else {
          int u = new_node(pos[it->second], suff-1);
          if (c >= 0)
            to[u][c] = new_node(&s[i], s.size()-i);
          to[u][t] = it->second;
          pos[it->second] += suff-1;
          len[it->second] -= suff-1;
          it->second = u;
          sl[last] = u;
          last = u;
        }
      }
      if (fork)
        fork = sl[fork];
      else
        suff--;
    }
  }
}

void dump(int d, int x)
{
  printf("%d\n", x);
  for (auto &c: to[x]) {
    int v = c.second;
    printf("%*s%s ", 2*d+2, "", string(pos[v]).substr(0, len[v]).c_str());
    dump(d+1, v);
  }
}

int main()
{
  int m;
  while (cin >> m) {
    vector<string> s(m);
    init();
    for (auto& x : s) {
      cin >> x;
      add(x);
    }
    dump(0, 0);
  }
}
