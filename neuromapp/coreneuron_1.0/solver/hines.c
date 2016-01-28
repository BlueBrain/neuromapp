/*
 * Neuromapp - hines.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * francesco.cremonesi@epfl.ch
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/solver/hines.c
 * \brief Implements the hines solver
 */

#include <stdio.h>
#include <assert.h>

#include "coreneuron_1.0/solver/hines.h"

#define VEC_A(i) (_nt->_actual_a[(i)])
#define VEC_B(i) (_nt->_actual_b[(i)])
#define VEC_D(i) (_nt->_actual_d[(i)])
#define VEC_RHS(i) (_nt->_actual_rhs[(i)])
#define VEC_V(i) (_nt->_actual_v[(i)])

void nrn_solve_minimal(NrnThread* _nt) {
	triang(_nt);
	bksub(_nt);
}

void triang(NrnThread* _nt) {
	double p;
	int i, i2, i3;
	i2 = _nt->ncell;
	i3 = _nt->end;

        assert(i2 >= 1);
        assert(i3 >= i2 + 1);

	for (i = i3 - 1; i >= i2; --i) {
		p = VEC_A(i) / VEC_D(i);
		VEC_D(_nt->_v_parent_index[i]) -= p * VEC_B(i);
		VEC_RHS(_nt->_v_parent_index[i]) -= p * VEC_RHS(i);
	}
}

void bksub(NrnThread* _nt) {
	int i, i1, i2, i3;
	i1 = 0;
	i2 = i1 + _nt->ncell;
	i3 = _nt->end;
	for (i = i1; i < i2; ++i) {
		VEC_RHS(i) /= VEC_D(i);
	}
	for (i = i2; i < i3; ++i) {
		VEC_RHS(i) -= VEC_B(i) * VEC_RHS(_nt->_v_parent_index[i]);
		VEC_RHS(i) /= VEC_D(i);
	}
}

#undef VEC_A
#undef VEC_B
#undef VEC_D
#undef VEC_RHS
#undef VEC_V