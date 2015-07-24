#ifndef _nah_
#define _nah_

#include <math.h>

#include "coreneuron_1.0/common/util/reader.h"

void mech_init_NaTs2_t(NrnThread *nt, Mechanism *ml);
void mech_state_NaTs2_t(NrnThread *nt, Mechanism *ml);
void mech_current_NaTs2_t(NrnThread *nt, Mechanism *ml);

void mech_init_Ih(NrnThread *nt, Mechanism *ml);
void mech_state_Ih(NrnThread *nt, Mechanism *ml);
void mech_current_Ih(NrnThread *nt, Mechanism *ml);

void mech_init_ProbAMPANMDA_EMS(NrnThread *nt, Mechanism *ml);
void mech_state_ProbAMPANMDA_EMS(NrnThread *nt, Mechanism *ml);
void mech_current_ProbAMPANMDA_EMS(NrnThread *nt, Mechanism *ml);

#endif // _nah_
