#include <stdio.h>

extern void foo(int *);
extern void bar(double *);

int main() {
    int x=3;
    double y=5;

    foo(&x);
    printf("x=%d\n",x);

    bar(&y);
    printf("y=%f\n",y);
}
