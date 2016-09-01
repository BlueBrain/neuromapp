/*
 * nest_kernel.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: schumann
 */

#include <numeric>

#include "nest/h5import/fakenestkernel/nest_kernel.h"

using namespace h5import;

void kernel_manager::create()
{
    kernel_instance = new kernel_manager;
}

void kernel_manager::destroy()
{
    delete kernel_instance;
}

size_t kernel_manager::mpi_manager::suggest_rank(const uint64_t& gid)
{
    return kernel().neuro_mpi_dist->suggest_group(gid);
}

size_t kernel_manager::node_manager::size()
{
    return kernel().neuro_vp_dist[kernel().vp_manager.get_thread_id()]->getglobalcells();
}
bool kernel_manager::node_manager::is_local_gid(const uint64_t& gid)
{
    return kernel().neuro_vp_dist[kernel().vp_manager.get_thread_id()]->isLocal(gid);
}

void kernel_manager::connection_manager::connect(const uint64_t& s_gid, const uint64_t& t_gid, const std::vector<double>& v)
{
    const int thrd = kernel().vp_manager.get_thread_id();
    if (num_connections.size()<=thrd) {
        num_connections.resize(thrd+1, 0);
        sum_values.resize(thrd+1, 0.);
    }
    num_connections[thrd]++;
    sum_values[thrd] = std::accumulate(v.begin(), v.end(), sum_values[thrd]);
}

kernel_manager* kernel_manager::kernel_instance = 0;
