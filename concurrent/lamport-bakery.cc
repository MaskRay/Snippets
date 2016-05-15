#include <algorithm>
#include <cstdio>
#include <thread>
#include <vector>
using namespace std;

#define REP(i, n) for (int i = 0; i < (n); i++)

const int N = 5;
bool enter[N];
int num[N];

void lamport_bakery(int i)
{
  enter[i] = true;
  num[i] = 1 + *max_element(num, num+N);
  enter[i] = false;
  REP(j, N) {
    while (enter[j]);
    while (num[j] && (num[j] < num[i] || num[j] == num[i] && j < i));
  }

  // critical section
  printf("%d\n", i);

  num[i] = 0;
}

int main()
{
  vector<thread> thrs;
  REP(i, 5)
    thrs.push_back(thread([=]() { lamport_bakery(i); }));
  for_each(thrs.begin(), thrs.end(), mem_fn(&thread::join));
}
