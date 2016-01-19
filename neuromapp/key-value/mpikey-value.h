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


#include <mpi.h>
#include <vector>

#include "key-value/kv-iface.h"

#ifndef mpikeyvalue_h
#define mpikeyvalue_h

class KeyValueArgs {
	public:
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

		KeyValueArgs(int procs, int threads, std::string backend, bool async,
				bool flash, int uc, float st, float md, float dt, int cg) :
					procs_(procs), threads_(threads), backend_(backend), async_(async),
					flash_(flash), usecase_(uc), st_(st), md_(md), dt_(dt), cg_(cg) {}

		KeyValueArgs() : procs_(1), threads_(1), backend_("map"), async_(false),
				flash_(false), usecase_(1), st_(1.0), md_(0.1), dt_(0.025), cg_(1) {}

		~KeyValueArgs(){}

};

class KeyValueStats {
	public:
		double mean_iops_;
		double mean_mbw_;
		double rank_iops_;
		double rank_mbw_;

		KeyValueStats() : mean_iops_(0), mean_mbw_(0), rank_iops_(0), rank_mbw_(0) {}
		~KeyValueStats() {}
};

class KeyValueBench {
private:

	int rank_;
	int num_procs_;
	int num_threads_;

	KeyValueIface * kv_store_;
	int voltages_size_;
	std::vector< std::vector<double> > voltages_;

	std::vector<int> gids_;

#ifdef SKV_IBM
	std::vector<skv_client_cmd_ext_hdl_t> ins_handles_;
	std::vector<skv_client_cmd_ext_hdl_t> rem_handles_;
#else
	std::vector<void *> ins_handles_;
	std::vector<void *> rem_handles_;
#endif

public:
	/** \fn KeyValueBench(int rank, int size)
	    \brief set up benchmark processes and threads
	    \param rank the rank of this processes
	    \param size the number of MPI processes
	 */
	KeyValueBench(int rank, int size);

	/** \fn ~KeyValueBench()
	    \brief delete ~KeyValueBench object
	 */
	~KeyValueBench();

	/** \fn getNumThreads()
	    \brief return the number of OpenMP threads
	 */
	int getNumThreads() { return num_threads_; }



	/** \fn parseArgs(int argc, char* argv[], KeyValueArgs &args)
	    \brief parse user arguments and set simulation parameters
	 */
	void parseArgs(int argc, char* argv[], KeyValueArgs &args);

	/** \fn init(KeyValueArgs &args)
	    \brief initialize the data structures needed for the simulation
	 */
	void init(KeyValueArgs &args);

	/** \fn cleanup(KeyValueArgs &args)
	    \brief clean up the data structures created for the simulation
	 */
	void cleanup(KeyValueArgs &args);

	/** \fn run(KeyValueArgs &args, KeyValueStats &stats)
	    \brief run the I/O simulation, calls init and cleanup as well
	 */
	void run(KeyValueArgs &args, KeyValueStats &stats);

	/** \fn run_loop(KeyValueArgs &args)
	    \brief run the I/O simulation as a single, large OpenMP loop
	 */
	void run_loop(KeyValueArgs &args, KeyValueStats &stats);

	/** \fn run_task(KeyValueArgs &args)
	    \brief run the I/O simulation as OpenMP tasks and dependencies
	 */
	void run_task(KeyValueArgs &args);


};

#endif
