/*
 * In-place inverse permutation
 *
 * Input
 * 4
 * 1 2 3 0
 */
#include <iostream>
#include <type_traits>
using namespace std;

#define FOR(i, a, b) for (remove_cv<remove_reference<decltype(b)>::type>::type i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

const long N = 100;
long a[N];

void huang_bing_chao(long a[], long n)
{
  while (n--) {
    long i = a[n];
    if (i >= 0) {
      i = -1;
      while (a[n] >= 0) {
        long t = a[n];
        a[n] = i;
        i = ~ n;
        n = t;
      }
    }
    a[n] = ~ i;
  }
}

int main()
{
  long n;
  while (cin >> n) {
    REP(i, n)
      cin >> a[i];
    huang_bing_chao(a, n);
    REP(i, n)
      cout << a[i] << ' ';
    cout << endl;
  }
}
