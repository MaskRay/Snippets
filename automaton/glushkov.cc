/*
 * Glushkov's construction algorithm
 *
 * Transforms a given regular expression into a finite automaton
 * S := [a-z] | S "|" S | S "*" | S S
 *
 * Input a word to specify the regular expression in the shape of S.
 *
 * firstpos, lastpos, nullable, followpos
 */
#include <algorithm>
#include <cctype>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <vector>
using namespace std;

#define ALL(x) (x).begin(), (x).end()

typedef map<long, set<long>> FollowPos;

struct Expr {
  vector<long> firstpos, lastpos;
  virtual void follow(FollowPos&) {}
  virtual bool nullable() const = 0;
  virtual ~Expr() {}
};

struct Literal : Expr {
  char lit;
  long pos;
  Literal(char lit, long pos) : lit(lit), pos(pos) {
    firstpos.push_back(pos);
    lastpos.push_back(pos);
  }
  virtual bool nullable() const override { return false; }
};

struct Star : Expr {
  Expr* x;
  Star(Expr* x) : x(x) {
    firstpos = x->firstpos;
    lastpos = x->lastpos;
  }
  void follow(FollowPos& followpos) override {
    for (auto i: lastpos)
      copy(ALL(firstpos), inserter(followpos[i], followpos[i].begin()));
    x->follow(followpos);
  }
  bool nullable() const override { return true; }
  ~Star() { delete x; }
};

struct Cat : Expr {
  Expr *l, *r;
  Cat(Expr* l, Expr* r) : l(l), r(r) {
    firstpos = l->firstpos;
    if (l->nullable())
      copy(ALL(r->firstpos), back_inserter(firstpos));
    if (r->nullable())
      lastpos = l->lastpos;
    copy(ALL(r->lastpos), back_inserter(lastpos));
  }
  void follow(FollowPos& followpos) override {
    for (auto i: l->lastpos)
      copy(ALL(r->firstpos), inserter(followpos[i], followpos[i].begin()));
    l->follow(followpos);
    r->follow(followpos);
  }
  bool nullable() const override { return l->nullable() && r->nullable(); }
  ~Cat() { delete l; delete r; }
};

struct Union : Expr {
  Expr *l, *r;
  Union(Expr* l, Expr* r) : l(l), r(r) {
    firstpos = l->firstpos;
    copy(ALL(r->firstpos), back_inserter(firstpos));
    lastpos = l->lastpos;
    copy(ALL(r->lastpos), back_inserter(lastpos));
  }
  void follow(FollowPos& followpos) override {
    l->follow(followpos);
    r->follow(followpos);
  }
  bool nullable() const override { return l->nullable() || r->nullable(); }
  ~Union() { delete l; delete r; }
};

int main()
{
  char a[100+1];
  long isp[127], icp[127];
  isp['\0'] = 0; icp['\0'] = 0;
  isp['*'] = 8;  icp['*'] = 7;
  isp['|'] = 4;  icp['|'] = 3;
  isp['@'] = 6;  icp['@'] = 5;
  isp['('] = 1;  icp['('] = 9;
                 icp[')'] = 1;
  while (cin >> a) {
    stack<Expr*> ex;
    stack<char> op;
    bool flag = false;
    op.push('\0');
    auto f = [&](char p) {
      for (; isp[op.top()] > icp[p]; op.pop())
        switch (op.top()) {
        case '*':
          ex.top() = new Star(ex.top());
          break;
        case '|': {
          Expr* r = ex.top();
          ex.pop();
          ex.top() = new Union(ex.top(), r);
          break;
        }
        case '@': {
          Expr* r = ex.top();
          ex.pop();
          ex.top() = new Cat(ex.top(), r);
          break;
        }
        }
      if (isp[op.top()] == icp[p])
        op.pop();
      else
        op.push(p);
    };
    for (char* p = a; ; p++) {
      if (isalpha(*p)) {
        if (flag)
          f('@');
        flag = true;
        ex.push(new Literal(*p, p-a));
      } else {
        if (flag && *p == '(')
          f('@');
        f(*p);
        flag = *p == '*' || *p == ')';
      }
      if (! *p) break;
    }

    Expr* root = new Cat(new Literal(0, -1), ex.top());
    FollowPos followpos;
    root->follow(followpos);
    vector<long> finals;
    map<vector<long>, long> id;
    queue<vector<long>> q;
    vector<long> u{-1};
    id[u] = 0;
    q.push(u);
    cout << "src positions , character , dest positions\n";
    while (q.size()) {
      u = q.front();
      q.pop();
      for (long i: u)
        if (binary_search(ALL(root->lastpos), i)) {
          finals.push_back(id[u]);
          break;
        }
      vector<long> v;
      for (long i: u)
        copy(ALL(followpos[i]), back_inserter(v));
      sort(ALL(v));
      v.erase(unique(ALL(v)), v.end());
      map<char, vector<long>> to;
      for (long i: v)
        to[a[i]].push_back(i);
      for (auto& it: to) {
        if (! id.count(it.second)) {
          long t = id.size();
          id[it.second] = t;
          q.push(it.second);
        }
        for (long i: u)
          cout << i << ' ';
        cout << it.first;
        for (long i: v)
          cout << ' ' << i;
        cout << endl;
      }
    }
    delete root;
  }
}
