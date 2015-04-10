#ifndef PRIMITIVE_OPS_H
#define PRIMITIVE_OPS_H

#include <cstddef>
#include <stdexcept>
#include "llc/ll_common.h"

namespace arith_op {
    enum arith_op { add, mul, div, sqrt };
}

/** Wrap underlying assembly for primitive arithmetic operation.
 *
 * Argument convention is:
 *     primitive_op<OP>::run(V &a1,V a2,...)
 * implements the arithmetic operation on values of type V
 *     a1 = OP(a1,a2,...)
 * where extra arguments are ignored.
 */

template <arith_op::arith_op op>
struct primitive_op {
    static void run(...) { throw std::invalid_argument("unsupported operation"); }
};

/** arith_op to string conversion */

std::string to_string(arith_op::arith_op);
std::ostream &operator<<(std::ostream &,arith_op::arith_op);

// architecture-specific specializations:

#if defined(__powerpc) || defined(_M_PPC) || defined(_ARCH_PPC)
#include "primitive_ops_powerpc.h"
#elif defined(__x86_64) || defined(_M_X64)
#include "primitive_ops_x86_64.h"
#else
#warning "unsupported archictecture"
#endif


#endif // ndef PRIMITIVE_OPS_H
