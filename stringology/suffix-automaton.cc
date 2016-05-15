/*
 * Suffix automaton of multiple strings / minimum directed acyclic word graph
 *
 * The first line contains an integer `m` : the number of strings
 * Each of the next `m` lines contains a string of lowercase letters
 *
 * Input
 * 3
 * abbabb
 * ababbba
 * bbabb
 *
 */
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

#define FOR(i, a, b) for (remove_cv<remove_reference<decltype(b)>::type>::type i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

const long N = 1000, AB = 26;

struct Node
{
  Node *f, *c[26];
  int len;
} pool[2*N], *allo;

Node* new_node()
{
  fill_n(allo->c, AB, nullptr);
  return allo++;
}

void init()
{
  allo = pool;
  new_node();
  pool->len = 0;
}

void add(const string& s)
{
  Node* tail = pool;
  for (char c: s) {
    c -= 'a';
    bool created = false;
    Node *p = tail, *x;
    if (p->c[c])
      x = p->c[c];
    else {
      created = true;
      x = new_node();
      x->len = p->len+1;
      for (; p && ! p->c[c]; p = p->f)
        p->c[c] = x;
    }
    if (! p)
      x->f = pool;
    else if (p->len+1 == p->c[c]->len) {
      if (created)
        x->f = p->c[c];
    } else {
      Node *q = p->c[c], *r = allo++;
      *r = *q;
      r->len = p->len+1;
      for (; p && p->c[c] == q; p = p->f)
        p->c[c] = r;
      q->f = r;
      if (created)
        x->f = r;
      else
        x = r;
    }
    tail = x;
  }
}

void dump()
{
  REP(i, allo-pool)
    REP(c, AB)
      if (pool[i].c[c])
        printf("%zd --%c--> %td\n", i, int('a'+c), pool[i].c[c]-pool);
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
    dump();
  }
}
