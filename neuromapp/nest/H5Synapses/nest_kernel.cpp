/*
 * nest_kernel.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: schumann
 */

#include "nest/H5Synapses/nest_kernel.h"

void nest::kernel_manager::create()
{
    kernel_instance = new kernel_manager;
}

void nest::kernel_manager::destroy()
{
    delete kernel_instance;
}

nest::index nest::kernel_manager::mpi_manager::suggest_rank(const int& gid)
{
    return kernel().neuro_mpi_dist->suggest_group(gid);
}

nest::index nest::kernel_manager::node_manager::size()
{
    return kernel().neuro_vp_dist[kernel().vp_manager.get_thread_id()]->getglobalcells();
}
bool nest::kernel_manager::node_manager::is_local_gid(const int& gid)
{
    return kernel().neuro_vp_dist[kernel().vp_manager.get_thread_id()]->isLocal(gid);
}

void nest::kernel_manager::connection_manager::connect(const int& s_gid, const int& t_gid, std::vector<double>& v)
{
    //assert(kernel().neuro_vp_dist[kernel().vp_manager.get_thread_id()]->isLocal(t_gid));

    const int thrd = kernel().vp_manager.get_thread_id();
    if (num_connections.size()<=thrd)
        num_connections.resize(thrd+1);
    num_connections[thrd]++;
    //maybe computation
}

nest::kernel_manager* nest::kernel_manager::kernel_instance = 0;
