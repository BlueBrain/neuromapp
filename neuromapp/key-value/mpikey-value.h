/*
 * Neuromapp - pool.h, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * @file neuromapp/key-value/mpikey-value.h
 * contains declaration for the KeyValueBench class and its related classes KeyValueArgs and KeyValueStats
 */


#ifndef mpikeyvalue_h
#define mpikeyvalue_h

#include <mpi.h>
#include <vector>

#include "key-value/kv-iface.h"
#include "key-value/meta.h"
#include "key-value/memory.h"

class KeyValueArgs {
private:
    int			procs_;
    int			threads_;
    std::string	backend_;
    bool 		async_;
    bool		flash_;
    int			usecase_;
    float		st_;
    float		md_;
    float		dt_;
    int			cg_;

public:
    explicit KeyValueArgs(int procs = 1, int threads = 1 , std::string backend = "map", bool async = false,
                 bool flash = false, int uc = 1, float st = 1., float md = 0.1, float dt = 0.025, int cg = 1) :
					procs_(procs), threads_(threads), backend_(backend), async_(async),
					flash_(flash), usecase_(uc), st_(st), md_(md), dt_(dt), cg_(cg) {}

    ~KeyValueArgs(){}


    inline int procs() const {
        return procs_;
    }

    inline int threads() const {
        return threads_;
    }

    inline std::string backend() const {
        return backend_;
    }

    inline bool async() const {
        return async_;
    }

    inline bool flash() const {
        return flash_;
    }

    inline int usecase() const{
        return usecase_;
    }

    inline float st() const{
        return st_;
    }

    inline float md() const{
        return md_;
    }

    inline float dt() const{
        return dt_;
    }

    inline int cg() const{
        return cg_;
    }

    inline int &procs()  {
        return procs_;
    }

    inline int &threads()  {
        return threads_;
    }

    inline std::string &backend()  {
        return backend_;
    }

    inline bool &async()  {
        return async_;
    }

    inline bool &flash()  {
        return flash_;
    }

    inline int &usecase() {
        return usecase_;
    }

    inline float &st() {
        return st_;
    }

    inline float &md() {
        return md_;
    }

    inline float &dt() {
        return dt_;
    }

    inline int &cg() {
        return cg_;
    }
};

class KeyValueStats {
	public:
		double mean_iops_;
		double mean_mbw_;
		double rank_iops_;
		double rank_mbw_;

		KeyValueStats() : mean_iops_(0.), mean_mbw_(0.), rank_iops_(0.), rank_mbw_(0.) {}
		~KeyValueStats() {}
};



template<keyvalue::selector h>
struct trait_handle;

template<>
struct trait_handle<keyvalue::selector::map>{
		typedef void* value_type;
};

template<>
struct trait_handle<keyvalue::selector::skv>{
#ifdef SKV_IBM
		typedef skv_client_cmd_ext_hdl_t value_type;
#endif
};

struct mpi{

    int rank_;
    int num_procs_;
    int num_threads_;
};


template<class T>
struct benchmark {
    typedef T value_type;


};



template<keyvalue::selector h = keyvalue::map>
class KeyValueBench {
private:

    // should be outside MPI later
	int rank_;
	int num_procs_;
	int num_threads_;

    // delete in the nex version
	KeyValueIface * kv_store_;

	std::vector<std::vector<double> > voltages_;
    std::vector<int > gids_ ;
    int voltages_size_;

    //the data
	std::vector<typename trait_handle<h>::value_type > ins_handles_;
	std::vector<typename trait_handle<h>::value_type> rem_handles_;

    typedef keyvalue::meta meta_type;
    keyvalue::group<meta_type> g;

public:
	/** \fn KeyValueBench(int rank, int size)
	    \brief set up benchmark processes and threads
	    \param rank the rank of this processes
	    \param size the number of MPI processes
	 */
	explicit KeyValueBench(int rank = 0, int size = 1) : rank_(rank), num_procs_ (size), num_threads_(1),
			kv_store_(NULL) {}

	/** \fn getNumThreads()
	    \brief return the number of OpenMP threads
	 */
	inline int getNumThreads() { return num_threads_; }

	/** \fn parseArgs(int argc, char* argv[], KeyValueArgs &args)
	    \brief parse user arguments and set simulation parameters
	 */
	inline void parseArgs(int argc, char* argv[], KeyValueArgs &args);

	/** \fn init(KeyValueArgs &args)
	    \brief initialize the data structures needed for the simulation
	 */
	inline void init(KeyValueArgs &args);

	/** \fn cleanup(KeyValueArgs &args)
	    \brief clean up the data structures created for the simulation
	 */
	inline void cleanup(KeyValueArgs &args);

	/** \fn run(KeyValueArgs &args, KeyValueStats &stats)
	    \brief run the I/O simulation, calls init and cleanup as well
	 */
	inline void run(KeyValueArgs &args, KeyValueStats &stats);

	/** \fn run_loop(KeyValueArgs &args)
	    \brief run the I/O simulation as a single, large OpenMP loop
	 */
	inline void run_loop(KeyValueArgs &args, KeyValueStats &stats);

    inline void run_loop_meta(KeyValueArgs &args, KeyValueStats &stats);


	/** \fn run_task(KeyValueArgs &args)
	    \brief run the I/O simulation as OpenMP tasks and dependencies
	 */
	inline void run_task(KeyValueArgs &args);
};

#include "key-value/mpikey-value.ipp"
#endif
