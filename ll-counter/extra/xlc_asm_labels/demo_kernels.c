#include "mark_line_xlc.h"

void foo(int *a) {
    ASM_MARK_XLC_FROM(start_foo);
    ++*a;
    ASM_MARK_XLC_TO(end_foo);
}

void bar(double *a) {
    ASM_MARK_XLC_FROM(start_bar);
    *a*=2.0;
    ASM_MARK_XLC_TO(end_bar);
}


