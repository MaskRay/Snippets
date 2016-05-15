#include <algorithm>
#include <cstdio>
#include <cstring>
#include <tuple>
#include <type_traits>
#include <utility>
using namespace std;

typedef pair<int, int> pii;
typedef tuple<int, int, int> tiii;
#define FOR(i, a, b) for (remove_cv<remove_reference<decltype(b)>::type>::type i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

const long N = 500, AB = 26;

int allo;
struct Node {
  int id, p, sl;
  tiii ch[AB];
} st[2*N];

int new_node(int p)
{
  st[allo].id = st[allo].sl = -1;
  st[allo].p = p;
  REP(i, AB)
    st[allo].ch[i] = tiii{-1, 0, 0};
  return allo++;
}

int fast_find(char *a, int x, int j, int k)
{
  while (j < k) {
    int y, jj, l;
    tie(y, jj, l) = st[x].ch[a[j]-'a'];
    if (j+l > k) {
      int z = new_node(x);
      st[x].ch[a[jj]-'a'] = tiii{z, jj, k-j};
      st[z].ch[a[jj+k-j]-'a'] = tiii{y, jj+k-j, l-k+j};
      st[y].p = z;
      return z;
    }
    x = y;
    j += l;
  }
  return x;
}

pii slow_find(char *a, int n, int x, int k)
{
  while (k < n && get<0>(st[x].ch[a[k]-'a']) >= 0) {
    int y, j, l, i;
    tie(y, j, l) = st[x].ch[a[k]-'a'];
    i = j;
    do i++, k++;
    while (i < j+l && k < n && a[i] == a[k]);
    if (i < j+l) {
      int z = new_node(x);
      st[x].ch[a[j]-'a'] = {z, j, i-j};
      st[z].ch[a[i]-'a'] = {y, i, l-i+j};
      st[y].p = z;
      return {z, k};
    }
    x = y;
  }
  return {x, k};
}

void suffix_tree(char *a, int n)
{
  allo = 0;
  new_node(-1);
  st[0].sl = 0;
  int fork = 0, k = 0, y;
  REP(i, n) {
    k = max(k, i);
    if (st[fork].sl < 0) {
      int t = st[fork].p, j, l;
      REP(c, AB)
        if (get<0>(st[t].ch[c]) == fork) {
          j = get<1>(st[t].ch[c]);
          l = get<2>(st[t].ch[c]);
          break;
        }
      if (! t) l--;
      st[fork].sl = fast_find(a, st[t].sl, k-l, k);
    }
    tie(fork, k) = slow_find(a, n, st[fork].sl, k);
    if (k == n)
      y = fork;
    else {
      y = new_node(fork);
      st[fork].ch[a[k]-'a'] = {y, k, n-k};
    }
    st[y].id = i;
  }
  //st[0].id = -1;
}

void dump(int d, char *a, int x)
{
  printf("%d", x);
  if (st[x].id >= 0)
    printf(" pos %d", st[x].id);
  puts("");
  REP(c, 26) {
    int y, j, l;
    tie(y, j, l) = st[x].ch[c];
    if (0 <= y && y < allo) {
      char t = a[j+l];
      a[j+l] = '\0';
      printf("%*s%s ", 2*(d+1), "", a+j);
      a[j+l] = t;
      dump(d+1, a, y);
    }
  }
}

int main()
{
  char a[999];
  while (fgets(a, sizeof a, stdin)) {
    int l = strlen(a);
    if (! l) break;
    a[--l] = '\0';
    suffix_tree(a, l);
    puts("=====");
    dump(0, a, 0);
  }
}
