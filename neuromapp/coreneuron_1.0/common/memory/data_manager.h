#ifndef _data_manager_
#define _data_manager_

void setup_nrnthreads_on_device(NrnThread *nt);
void update_nrnthreads_on_host(NrnThread *nt);
void compare(NrnThread *nt, NrnThread *nt_old);

#endif // _data_manager_
