// while :; do timeout 3 ./a | awk '{b=b $0 "\n"; if(x>$1){bad=1;print "bad";} else x=$1;} END{if(bad)print b}'; [[ $? -eq 143 ]] && echo timeout; done
// Ref: A Practical Concurrent Binary Search Tree
// concurrent AVL tree (no duplicates)
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <mutex>
#include <thread>
using namespace std;

#define REP(i, n) FOR(i, 0, n)
#define REP1(i, n) FOR(i, 1, n+1)
#define FOR(i, a, b) for (int i = (a); i < (b); i++)

typedef int Key;

const int UNLINKED = 1;
const int GROWING = 2;
const int SHRINKING = 4;
const int GROW_COUNT_MASK = 0xff << 3;
const int GROW_COUNT_INCR = 1 << 3;
const int SHRINK_COUNT_INCR = 1 << 11;

const int UNLINK_REQUIRED = -1;
const int REBALANCE_REQUIRED = -2;
const int NOTHING_REQUIRED = -3;

struct Node
{
  Key key;
  void *val;
  int version, height;
  mutex lock;
  Node *parent, *child[2];
  Node(Key key, void *val, int version, int height, Node *parent, Node *left, Node *right)
    : key(key), val(val), version(version), height(height), parent(parent), child{left, right} {}
  bool is_unlinked() {
    return version == UNLINKED;
  }
  bool has_null_child() {
    return ! child[0] || ! child[1];
  }
};

int compare(int a, int b)
{
  return a-b;
}

void print(int d, Node *x)
{
  if (x) {
    print(d+1, x->child[0]);
    printf("%*s%d %d(%p)\n", d*2, "", x->key, x->height, x->val);
    print(d+1, x->child[1]);
  }
}

class ConcurrentBST
{
public:
  ConcurrentBST()
    : root_holder_(Key(), nullptr, 0, 0, nullptr, nullptr, nullptr) {}
  ~ConcurrentBST() {
  }

  void *get(Key key) {
    void *ret;
    while ((ret = attempt_get(&root_holder_, 1, 0, key)) == RETRY);
    return ret;
  }

  void *put(Key key, void *val) {
    void *ret;
    while ((ret = attempt_put(&root_holder_, 1, 0, key, val)) == RETRY);
    return ret;
  }

  // use garbage collection to truly delete nodes
  void *remove(Key key) {
    void *ret;
    while ((ret = attempt_put(&root_holder_, 1, 0, key, nullptr)) == RETRY);
    return ret;
  }

  void print() {
    ::print(0, root_holder_.child[1]);
  }

private:
  static constexpr Node *RETRY = (Node *)-1;
  Node root_holder_;

  int node_condition(Node *node) {
    Node *l = node->child[0], *r = node->child[1];
    if ((! l || ! r) && ! node->val)
      return UNLINK_REQUIRED;
    int h = node->height,
        hl = l ? l->height : 0,
        hr = r ? r->height : 0,
        hh = 1 + max(hl, hr),
        bal = hl - hr;
    if (bal < -1 || bal > 1)
      return REBALANCE_REQUIRED;
    return h == hh ? NOTHING_REQUIRED : hh;
  }

  bool is_shrinking_or_unlinked(int v) {
    return v & (SHRINKING | UNLINKED);
  }

  // parent, node and r must be locked on entry
  // rl may be NULL
  Node *zag_locked(Node *p, Node *n, Node *r, Node *rl, int hl, int hrl, int hrr, int d) {
    n->version |= SHRINKING;
    r->version |= GROWING;

    n->child[!d] = rl;
    r->child[d] = n;
    if (p->child[0] == n) p->child[0] = r;
    else p->child[1] = r;

    r->parent = p;
    n->parent = r;
    if (rl) rl->parent = n;

    int h1 = 1 + max(hl, hrl);
    n->height = h1;
    r->height = 1 + max(h1, r->child[!d] ? r->child[!d]->height : 0);

    r->version = (r->version | 7) + 1;
    n->version = (n->version | (1 << 11) - 1) + 1;

    int bal = hl - hrl;
    if (bal < -1 || bal > 1 || ! n->val && (! hl || ! rl))
      return n;
    bal = h1 - hrr;
    if (bal < -1 || bal > 1 || ! n->val && ! hrr)
      return r;
    return fix_height_locked(p);
  }

  // d == 0 => left over right; d == 1 => right over left
  Node *zigzag_locked(Node *p, Node *n, Node *r, Node *rl, int hl, int hrr, int d) {
    Node *rll = rl->child[d], *rlr = rl->child[!d];
    int hrll = rll ? rll->height : 0, hrlr = rlr ? rlr->height : 0;

    n->version |= SHRINKING;
    r->version |= GROWING;
    rl->version |= SHRINKING;

    n->child[!d] = rll;
    r->child[d] = rlr;
    rl->child[d] = n;
    rl->child[!d] = r;
    if (p->child[0] == n) p->child[0] = rl;
    else p->child[1] = rl;

    rl->parent = p;
    r->parent = rl;
    n->parent = rl;
    if (rll) rll->parent = n;
    if (rlr) rlr->parent = r;

    int hn1 = 1 + max(hl, hrll);
    n->height = hn1;
    int hr1 = 1 + max(hrlr, hrr);
    r->height = hr1;
    rl->height = 1 + max(hn1, hr1);

    rl->version = (rl->version | (1 << 11) - 1) + 1;
    r->version = (r->version | 7) + 1;
    n->version = (n->version | (1 << 11) - 1) + 1;
    return fix_height_locked(p);
  }

  // parent and node must be locked on entry
  bool attempt_unlink_locked(Node *parent, Node *node) {
    if (! node->has_null_child())
      return false;
    Node *c = node->child[0] ? node->child[0] : node->child[1];
    if (c) {
      lock_guard<mutex> lock(c->lock);
      if (parent->child[0] == node)
        parent->child[0] = c;
      else
        parent->child[1] = c;
      c->parent = parent;
    } else {
      if (parent->child[0] == node)
        parent->child[0] = c;
      else
        parent->child[1] = c;
    }
    node->child[0] = node->child[1] = nullptr;
    node->val = nullptr;
    node->version = UNLINKED;
    return true;
  }

  // fix height of the locked damaged node
  // returns a damaged node
  Node *fix_height_locked(Node *node) {
    int condition = node_condition(node);
    switch (condition) {
    case REBALANCE_REQUIRED:
    case UNLINK_REQUIRED:
      return node;
    case NOTHING_REQUIRED:
      return nullptr;
    default:
      node->height = condition;
      return node->parent == &root_holder_ ? nullptr : node->parent;
    }
  }

  Node *rebalance_to_d_locked(Node *parent, Node *node, Node *r, int hl0, int d) {
    lock_guard<mutex> lock(r->lock);
    int hr = r->height;
    if (hl0 - hr >= -1)
      return node;
    Node *rl = r->child[d];
    int hrl0 = rl ? rl->height : 0, hrr0 = r->child[!d] ? r->child[!d]->height : 0;
    if (hrr0 >= hrl0)
      return zag_locked(parent, node, r, rl, hl0, hrl0, hrr0, d);
    lock_guard<mutex> lock2(rl->lock);
    if (hrr0 >= hrl0)
      return zag_locked(parent, node, r, rl, hl0, hrl0, hrr0, d);
    return zigzag_locked(parent, node, r, rl, hl0, hrr0, d);
  }

  // parent and node must be locked on entry
  // @return a damaged node
  Node *rebalance_locked(Node *parent, Node *node) {
    if (! node->val && node->has_null_child()) {
      assert(attempt_unlink_locked(parent, node));
      return fix_height_locked(parent);
    }
    Node *l = node->child[0], *r = node->child[1];
    int h = node->height,
        hl = l ? l->height : 0,
        hr = r ? r->height : 0,
        hh = 1 + max(hl, hr),
        bal = hl - hr;
    if (bal < -1)
      return rebalance_to_d_locked(parent, node, r, hl, 0);
    if (bal > 1)
      return rebalance_to_d_locked(parent, node, l, hr, 1);
    if (h == hh)
      return nullptr;
    node->height = h;
    return fix_height_locked(parent);
  }

  void fix_height_and_rebalance(Node *node) {
    while (node && ! node->is_unlinked() && node->parent) {
      switch (node_condition(node)) {
      case NOTHING_REQUIRED:
        return;
      case REBALANCE_REQUIRED:
      case UNLINK_REQUIRED: {
        Node *parent = node->parent;
        lock_guard<mutex> lock(parent->lock);
        if (! parent->is_unlinked() && node->parent == parent) {
          lock_guard<mutex> lock2(node->lock);
          node = rebalance_locked(parent, node);
        }
        // else RETRY
        break;
      }
      default: {
        lock_guard<mutex> lock(node->lock);
        node = fix_height_locked(node);
        break;
      }
      }
    }
  }

  void wait_shrinking(Node *node) {
    int version = node->version;
    if (node->version & SHRINKING) {
      lock_guard<mutex> lock(node->lock);
      assert(node->version != version);
    }
  }

  void *attempt_get(Node *node, int dir, int version, Key key) {
    for(;;) {
      Node *child = node->child[dir];
      if (node->version != version)
        return RETRY;
      if (! child)
        return nullptr;
      int dir2 = compare(key, child->key);
      if (! dir2)
        return child->val;
      int ch_v = child->version;
      if (is_shrinking_or_unlinked(ch_v))
        wait_shrinking(child);
      else if (node->child[dir] == child) {
        if (node->version != version)
          return RETRY;
        void *ret = attempt_get(child, dir2 > 0, ch_v, key);
        if (ret != RETRY)
          return ret;
      }
    }
  }

  void *attempt_insert(Node *node, int dir, int version, Key key, void *val) {
    {
      lock_guard<mutex> lock(node->lock);
      if (node->version != version)
        return RETRY;
      node->child[dir] = new Node(key, val, 0, 1, node, nullptr, nullptr);
    }
    fix_height_and_rebalance(node);
    return nullptr;
  }

  // @return old value
  void *attempt_put(Node *node, int dir, int version, Key key, void *val) {
    void *ret = RETRY;
    do {
      Node *child = node->child[dir];
      if (node->version != version)
        return RETRY;
      if (! child) {
        if (! val) // nothing to remove
          return nullptr;
        // insert
        ret = attempt_insert(node, dir, version, key, val);
      } else {
        int dir2 = compare(key, child->key);
        if (! dir2) {
          if (val) { // update
            // TODO rebalance
            void *old;
            {
              lock_guard<mutex> lock(child->lock);
              if (node->is_unlinked() || child->parent != node)
                continue;
              old = child->val;
              child->val = val;
            }
            fix_height_and_rebalance(child);
            return old;
          }
          // remove
          ret = attempt_remove(node, child);
        } else {
          int ch_v = child->version;
          if (is_shrinking_or_unlinked(ch_v))
            wait_shrinking(node);
          else if (node->child[dir] == child) {
            if (node->version != version)
              return RETRY;
            ret = attempt_put(child, dir2 > 0, ch_v, key, val);
          }
        }
      }
    } while (ret == RETRY);
    return ret;
  }

  void *attempt_remove(Node *parent, Node *node) {
    void *ret;
    if (! node->has_null_child()) {
      lock_guard<mutex> lock(node->lock);
      if (node->version == UNLINKED || node->has_null_child())
        return RETRY;
      ret = node->val;
      node->val = nullptr;
    } else {
      lock_guard<mutex> lock(parent->lock);
      if (parent->version == UNLINKED || node->version == UNLINKED || node->parent != parent)
        return RETRY;
      lock_guard<mutex> lock2(node->lock);
      ret = node->val;
      node->val = nullptr;
      attempt_unlink_locked(parent, node);
    }
    return ret;
  }
};

long k[100000];

int main()
{
  iota(k, k+sizeof(k)/sizeof(*k), 1);
  random_shuffle(k,  k+sizeof(k)/sizeof(*k));

  ConcurrentBST bst;
  //const long k[] = {3,5,4,1,2,7,6};
  thread add([&]() {
            //const long v[] = {10,20,30,40,50,60};
  REP(i, sizeof(k)/sizeof(*k))
            REP(j, 100)
              bst.put(k[i], (void *)(k[i]*16));
            });

  thread remove([&]() {
            //const long v[] = {10,20,30,40,50,60};
  REP(i, sizeof(k)/sizeof(*k))
            REP(j, 100)
              bst.remove(k[i]);
            });

  REP(i, sizeof(k)/sizeof(*k))
    REP(j, 100)
    bst.put(k[i], (void *)k[i]);
  //bst.remove(2);
  //bst.put(3, (void*)4);

  add.join();
  remove.join();
  bst.print();

  //REP(i, sizeof(k)/sizeof(*k))
  //  fprintf(stderr, "%d %p\n", k[i], bst.remove(k[i]));
  //puts("---");
  bst.print();
}

