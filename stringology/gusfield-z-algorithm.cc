/*
 * Gusfield's Z algorithm
 * For each suffix of 'text', calculate the longest common prefix with 'pattern'
 *
 * The first line contains 'pattern' and the second line 'text'
 *
 * Input
 * ababab
 * ababbabba
 */
#include <iostream>
#include <type_traits>
using namespace std;

#define FOR(i, a, b) for (remove_cv<remove_reference<decltype(b)>::type>::type i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

const long N = 100;
char pat[N+1], text[N+1];
long pref[N], pref2[N];

void prefixes(const char a[])
{
  for (long f, g = 0, i = 1; a[i]; i++)
    if (i < g && pref[i-f] != g-i)
      pref[i] = min(pref[i-f], g-i);
    else {
      g = max(g, f = i);
      while (a[g] && a[g] == a[g-f])
        g++;
      pref[i] = g-f;
    }
}

void prefixes2(const char a[], const char b[])
{
  for (long f, g = 0, i = 0; b[i]; i++)
    if (i < g && pref[i-f] != g-i)
      pref2[i] = min(pref[i-f], g-i);
    else {
      g = max(g, f = i);
      while (b[g] && b[g] == a[g-f])
        g++;
      pref2[i] = g-f;
    }
}

int main()
{
  while (cin >> pat >> text) {
    prefixes(pat);
    prefixes2(pat, text);
    cout << "longest common prefix of pattern and pattern[i..]\n";
    for (long i = 1; pat[i]; i++)
      cout << i << '\t' << pref[i] << '\n';
    cout << "longest common prefix of pattern and text[i..]\n";
    for (long i = 0; text[i]; i++)
      cout << i << '\t' << pref2[i] << '\n';
  }
}
