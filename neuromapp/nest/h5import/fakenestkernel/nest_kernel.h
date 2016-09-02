
#ifdef _OPENMP
#include <omp.h>
#endif

#include <mpi.h>
#include <stdint.h>

#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

#ifndef NESTKERNEL_CLASS
#define NESTKERNEL_CLASS

namespace h5import
{
    typedef size_t index;
    struct kernel_manager
    {
        static kernel_manager* kernel_instance;

        environment::nestdistribution neuro_mpi_dist;
        std::vector< environment::nestdistribution* > neuro_vp_dist;

        struct mpi_manager
        {
            int rank_;
            int size_;

            mpi_manager( const int& rank, const int& size ): rank_(rank), size_(size)
            {
                //MPI_Comm_size(MPI_COMM_WORLD, &size);
                //MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            }

            inline index get_rank()
            {
                return rank_;
            }
            inline index get_num_processes()
            {
                return size_;
            }
            index suggest_rank( const index& gid );
        } mpi_manager;

        struct node_manager
        {
            index size();
            bool is_local_gid( const index& gid );
        } node_manager;

        struct vp_manager
        {
			size_t nthreads_;
			vp_manager( const size_t& nthreads ): nthreads_( nthreads )
			{}
            inline index get_thread_id()
            {
                #ifdef _OPENMP
                return omp_get_thread_num();
                #else
                return 0;
                #endif
            }
            inline const size_t& get_num_threads() const
            {
                return nthreads_;
            }
        } vp_manager;

        struct connection_manager
        {
            std::vector< long > num_connections;
            std::vector< double > sum_values;
            void connect( const index& s_gid, const index& t_gid, const std::vector< double >& v );
			connection_manager( const size_t nthreads ):
				num_connections( nthreads ),
				sum_values( nthreads )
			{};
        } connection_manager;
		
		kernel_manager( const index& nneurons, const size_t& nthreads, const int& rank, const int& size ):
			neuro_mpi_dist( size, rank, nneurons ),
			mpi_manager( rank, size ),
			vp_manager( nthreads ),
			connection_manager( nthreads ) 
		{
			for ( int thrd=0; thrd<nthreads; thrd++ )
				neuro_vp_dist.push_back( new environment::nestdistribution( nthreads, thrd, &neuro_mpi_dist ) );
		}
		~kernel_manager()
		{
			for ( int thrd=0; thrd<vp_manager.get_num_threads(); thrd++ )
				delete neuro_vp_dist[ thrd ];
		}

        static void create( const index& nneurons, const size_t& nthreads, const int& rank, const int& size );
        static void destroy();
    };

    struct kernel_env
    {
        kernel_env( const index& nneurons, const size_t& nthreads, const int& rank, const int& size )
        {
            kernel_manager::create( nneurons, nthreads, rank, size );
        }
        ~kernel_env()
        {
            kernel_manager::destroy();
        }
    };
	
	inline bool kernel_available()
	{
		return kernel_manager::kernel_instance != NULL;
	}

    inline kernel_manager& kernel()
    {
        return *kernel_manager::kernel_instance;
    }
};

#endif
