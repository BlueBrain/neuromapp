/*
 * nest_kernel.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: schumann
 */

#include <numeric>

#include "nest/h5import/fakenestkernel/nest_kernel.h"

using namespace h5import;

void kernel_manager::create( const index& nneurons, const size_t& nthreads, const int& rank, const int& size )
{
    kernel_instance = new kernel_manager( nneurons, nthreads, rank, size );
}

void kernel_manager::destroy()
{
    delete kernel_instance;
	kernel_instance = NULL;
}

size_t kernel_manager::mpi_manager::suggest_rank( const index& gid )
{
    return kernel().neuro_mpi_dist.suggest_group( gid );
}

size_t kernel_manager::node_manager::size()
{
    return kernel().neuro_vp_dist[ kernel().vp_manager.get_thread_id() ]->getglobalcells();
}
bool kernel_manager::node_manager::is_local_gid( const index& gid )
{
    return kernel().neuro_vp_dist[ kernel().vp_manager.get_thread_id() ]->isLocal( gid );
}

void kernel_manager::connection_manager::connect( const index& s_gid, const index& t_gid, const std::vector<double>& v )
{
    const size_t thrd = kernel().vp_manager.get_thread_id();
    
    num_connections[ thrd ]++;
    sum_values[ thrd ] = std::accumulate( v.begin(), v.end(), sum_values[ thrd ] );
}

kernel_manager* kernel_manager::kernel_instance = NULL;
