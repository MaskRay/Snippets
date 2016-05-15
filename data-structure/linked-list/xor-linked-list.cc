/*
 * XOR linked list
 *
 * https://en.wikipedia.org/wiki/XOR_linked_list
 */
#include <stdio.h>
#include <stdint.h>

struct Node {
  int value;
  intptr_t link;
};

Node* tail = NULL;

Node* newnode(int value)
{
  auto x = new Node{value, intptr_t(tail)};
  if (tail)
    tail->link = (intptr_t)tail->link ^ (intptr_t)x;
  return tail = x;
}

void iter()
{
  Node *x = NULL, *y = tail, *z;
  while (y) {
    printf("%d\n", y->value);
    z = (Node*)(y->link ^ (intptr_t)x);
    x = y;
    y = z;
  }
}

bool f(Node *x)
{
  return x->value % 2 == 1;
}

void filter(bool (*predicate)(Node *))
{
  Node *x = NULL, *y = tail, *z;
  while (y) {
    z = (Node *)(y->link ^ (intptr_t)x);
    if (! predicate(y)) {
      if (x)
        x->link = (x->link ^ (intptr_t)y ^ (intptr_t)z);
      else
        tail = z;
      if (z)
        z->link = (z->link ^ (intptr_t)y ^ (intptr_t)x);
      y = z;
    } else {
      x = y;
      y = z;
    }
  }
}

int main()
{
  for (int i = 0; i < 9; i++)
    newnode(i);
  filter(f);
  iter();
}
