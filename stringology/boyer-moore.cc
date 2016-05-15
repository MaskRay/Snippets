#include <cstdio>
#include <algorithm>
#include <cstring>
using namespace std;

const int SIZE = 1000;

#define OUTPUT(x) printf("%d\n", (x))

void suffixes(const char *a, int m, int *suff)
{
    suff[m-1] = m;
    for (int g = m-1, f, i = m-1; --i >= 0; )
        if (i > g && suff[i+m-1-f] != i-g)
            suff[i] = min(suff[i+m-1-f], i-g);
        else {
            f = i;
            g = min(g, i);
            while (g >= 0 && a[g] == a[g+m-1-f]) g--;
            suff[i] = f-g;
        }
}

void preBmBc(const char *a, int m, int *bc)
{
    fill(bc, bc+sizeof(bc)/sizeof(*bc), 0);
    for (int i = 0; i < m; i++)
        bc[a[i]] = m-1-i;
}

void preBmGs(const char *a, int m, int *gs)
{
    static int suff[SIZE];
    suffixes(a, m, suff);
    fill(gs, gs+m, m);
    for (int j = 0, i = m; --i >= 0; )
        if (suff[i] == i+1)
            for (; j < m-1-i; j++)
                gs[j] = m-1-i;
    for (int i = 0; i < m-1; i++)
        gs[m-1-suff[i]] = m-1-i;
}

void boyer_moore(const char *needle, int m, const char *haystack, int n)
{
    static int bc[SIZE], gs[SIZE];
    preBmBc(needle, m, bc);
    preBmGs(needle, m, gs);
    for (int i, j = 0; j <= n-m; ) {
        for (i = m-1; i >= 0 && needle[i] == haystack[j+i]; i--);
        if (i < 0) {
            OUTPUT(j);
            j += gs[0];
        } else
            j += max(gs[i], bc[haystack[j+i]]-m+1-i);
    }
}

int main()
{
    char a[100], b[100];
    int suff[100];
    while (scanf("%s%s", a, b) == 2){
        int m = strlen(a), n = strlen(b);
        boyer_moore(a, m, b, n);
    }
    return 0;
}
