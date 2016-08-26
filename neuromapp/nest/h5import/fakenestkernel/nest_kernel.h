#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"
#include <omp.h>
#include <mpi.h>

#ifndef NESTKERNEL_CLASS
#define NESTKERNEL_CLASS

namespace nest
{
    typedef size_t index;
    struct kernel_manager
    {
        static kernel_manager* kernel_instance;

        environment::nestdistribution* neuro_mpi_dist;
        std::vector<environment::nestdistribution*> neuro_vp_dist;

        struct mpi_manager
        {
            int rank;
            int size;

            mpi_manager(): size(1), rank(0)
            {
                MPI_Comm_size(MPI_COMM_WORLD, &size);
                MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            }

            inline index get_rank()
            {
                return rank;
            }
            inline index get_num_processes()
            {
                return size;
            }
            index suggest_rank(const int& gid);
        } mpi_manager;

        struct node_manager
        {
            index size();
            bool is_local_gid(const int& gid);
        } node_manager;

        struct connection_manager
        {
            std::vector<long> num_connections;
            void connect(const int& s_gid, const int& t_gid, std::vector<double>& v);
        } connection_manager;

        struct vp_manager
        {
            inline index get_thread_id()
            {
                #ifdef _OPENMP
                return omp_get_thread_num();
                #else
                return 0;
                #endif
            }
            inline index get_max_threads()
            {
                #ifdef _OPENMP
                return omp_get_max_threads();
                #else
                return 1;
                #endif
            }
        } vp_manager;

        inline void set_mpi_dist(environment::nestdistribution* mpi_dist)
        {
            neuro_mpi_dist = mpi_dist;
        }
        inline void set_vp_dist(const int& thrd, environment::nestdistribution* vp_dist)
        {
            if (thrd>=neuro_vp_dist.size())
                neuro_vp_dist.resize(thrd+1);
            neuro_vp_dist[thrd] = vp_dist;
        }

        static void create();
        static void destroy();
    };

    struct kernel_env
    {
        kernel_env()
        {
            kernel_manager::create();
        }
        ~kernel_env()
        {
            kernel_manager::destroy();
        }
    };

    inline nest::kernel_manager& kernel()
    {
        return *kernel_manager::kernel_instance;
    }
};

#endif
