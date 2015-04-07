#include "ll_common.h"
#include "unroll.h"
#include "../power8_latency/kernels_asm.h"

using namespace llc;

void check_asm_unroll() {
    ASM_LABEL("pre-unroll");
    serial_arithmetic<double,arith_op::add,80>::run();
    ASM_LABEL("post-unroll");
}

