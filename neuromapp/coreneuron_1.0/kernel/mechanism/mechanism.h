/*
 * Neuromapp - mechanism.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * paramod.kumbhar@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/kernel/mechanism/mechanism.h
 * Declare the different kernel of coreneuron 1.0
*/

#ifndef MAPP_KERNEL_MECHANISM_
#define MAPP_KERNEL_MECHANISM_

#include <math.h>

#include "coreneuron_1.0/common/util/reader.h"

/** \fn mech_init_NaTs2_t(NrnThread *nt, Mechanism *ml)
    \brief initialisation kernel for the NaTs2_t channel mechanism
    \param nt data structure
    \param ml the looking mechanism
 */
void mech_init_NaTs2_t(NrnThread *nt, Mechanism *ml);

/** \fn mech_state_NaTs2_t(NrnThread *nt, Mechanism *ml)
    \brief state kernel for the NaTs2_t channel mechanism
    \param nt data structure
    \param ml the looking mechanism
 */
void mech_state_NaTs2_t(NrnThread *nt, Mechanism *ml);

/** \fn mech_current_NaTs2_t(NrnThread *nt, Mechanism *ml)
    \brief current kernel for the NaTs2_t channel mechanism
    \param nt data structure
    \param ml the looking mechanism
 */
void mech_current_NaTs2_t(NrnThread *nt, Mechanism *ml);

/** \fn mech_init_Ih(NrnThread *nt, Mechanism *ml)
    \brief initialisation kernel for the Ih channel mechanism
    \param nt data structure
    \param ml the looking mechanism
 */
void mech_init_Ih(NrnThread *nt, Mechanism *ml);

/** \fn mech_state_Ih(NrnThread *nt, Mechanism *ml)
    \brief state kernel for the Ih channel mechanism
    \param nt data structure
    \param ml the looking mechanism
 */
void mech_state_Ih(NrnThread *nt, Mechanism *ml);

/** \fn mech_current_Ih(NrnThread *nt, Mechanism *ml)
    \brief current kernel for the Ih channel mechanism
    \param nt data structure
    \param ml the looking mechanism
 */
void mech_current_Ih(NrnThread *nt, Mechanism *ml);

/** \fn mech_init_ProbAMPANMDA_EMS(NrnThread *nt, Mechanism *ml)
    \brief initialisation kernel for the ProbAMPANMDA_EMS synapse mechanism
    \param nt data structure
    \param ml the looking mechanism
 */
void mech_init_ProbAMPANMDA_EMS(NrnThread *nt, Mechanism *ml);

/** \fn mech_state_ProbAMPANMDA_EMS(NrnThread *nt, Mechanism *ml)
    \brief state kernel for the ProbAMPANMDA_EMS synapse mechanism
    \param nt data structure
    \param ml the looking mechanism
 */
void mech_state_ProbAMPANMDA_EMS(NrnThread *nt, Mechanism *ml);

/** \fn mech_current_ProbAMPANMDA_EMS(NrnThread *nt, Mechanism *ml)
    \brief current kernel for the ProbAMPANMDA_EMS synapse mechanism
    \param nt data structure
    \param ml the looking mechanism
 */
void mech_current_ProbAMPANMDA_EMS(NrnThread *nt, Mechanism *ml);

#endif