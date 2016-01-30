/*
 * Neuromapp - tools.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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
 * @file neuromapp/key-value/utils/tools.h
 * \brief basic shell for mpi
 */

#ifndef MAPP_TOOLS_H
#define MAPP_TOOLS_H

#include <mpi.h>
#include <vector>
#include <numeric>
#include <functional>

namespace keyvalue {
    namespace utils{

    class master_mpi{
    private:
        master_mpi(){
            MPI::Init();
            s = MPI::COMM_WORLD.Get_size();
            r = MPI::COMM_WORLD.Get_rank();
        };

        master_mpi(master_mpi const& copy); //not implemented
        master_mpi& operator=(master_mpi const& copy); //not implemented

    public:

        ~master_mpi(){
            MPI::Finalize();
        }

        static master_mpi& getInstance(){
            static master_mpi instance;
            return instance;
        }

        int size() const {
            return s;
        }

        int rank() const {
            return r;
        }

    private:
        int s;
        int r;
    };
    /** singleton call once at the beginning and destroy at the end */
    static master_mpi const& master = master_mpi::getInstance(); 

    /** \fun accumulate(InputIt first, InputIt last, T init) 
    
    a kind of MPI accumulate
      it should be generic blabla, not type safe only work with DOUBLE */
    template<class InputIt, class T>
    inline T accumulate(InputIt first, InputIt last, T init){
        T tmp(0.), rec_mpi(0.);
        tmp  = std::accumulate(first,last, init);
        MPI_Reduce(&tmp, &rec_mpi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );
        return tmp;
    }
    
    /** a tiny timer only MPI now */
    class timer{
        public:
        explicit timer(double t0 = 0.):t(t0){}
        
        inline void tic(){
            t = MPI_Wtime();
        }
        
        inline void toc(){
            t = MPI_Wtime() - t;
        }

        inline double time() {
            return t;
        }
    
        private:
        double t;
    };
    
    } //end namespave utils
} //end namespace keyvalue

#endif