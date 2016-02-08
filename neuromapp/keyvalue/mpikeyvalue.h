/*
 * Neuromapp - pool.h, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details..  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/keyvalue/mpikeyvalue.h
 * contains declaration for the KeyValueBench class and its related classes argvs and stats
 */


#ifndef mpikeyvalue_h
#define mpikeyvalue_h

#include <mpi.h>
#include <vector>
#include <numeric>
#include <functional>

#include "keyvalue/kv-iface.h"
#include "keyvalue/meta.h"
#include "keyvalue/memory.h"
#include "keyvalue/utils/tools.h"
#include "keyvalue/map/map_store.h"
#include "keyvalue/utils/trait.h"

class argvs {
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
    int         voltages_size_;

public:
    explicit argvs(int procs = 1, int threads = 1 , std::string backend = "map", bool async = false,
                 bool flash = false, int uc = 1, float st = 1., float md = 0.1, float dt = 0.025, int cg = 1) :
					procs_(procs), threads_(threads), backend_(backend), async_(async),
					flash_(flash), usecase_(uc), st_(st), md_(md), dt_(dt), cg_(cg),
                    voltages_size_(usecase_*4096/2.5*350){}

    inline int voltage_size() const{
        return voltages_size_;
    }
    
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
    
    inline int &voltage_size(){
        return voltages_size_;
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

class stats {
	public:
    stats():mean_iops_(0.), mean_mbw_(0.), rank_iops_(0.), rank_mbw_(0.){}

    inline double& mean_iops(){
        return mean_iops_;
    }

    inline double const& mean_iops() const{
        return mean_iops_;
    }

    inline double& mean_mbw(){
        return mean_mbw_;
    }

    inline double const& mean_mbw() const{
        return mean_mbw_;
    }

    inline double& rank_iops(){
        return rank_iops_;
    }

    inline double const& rank_iops() const{
        return rank_iops_;
    }

    inline double& rank_mbw(){
        return rank_mbw_;
    }

    inline double const& rank_mbw() const{
        return rank_mbw_;
    }

    private:
        double mean_iops_;
		double mean_mbw_;
		double rank_iops_;
		double rank_mbw_;
};

template<keyvalue::selector h>
struct trait_handle;
/*
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
*/
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

public:
	/** \fn KeyValueBench(int rank, int size)
	    \brief set up benchmark processes and threads
	    \param rank the rank of this processes
	    \param size the number of MPI processes
	 */
	explicit KeyValueBench(int rank = 0, int size = 1) : rank_(rank), num_procs_ (size), num_threads_(1),kv_store_(NULL){
            rank_ = mapp::master.rank();
            num_procs_ = mapp::master.size();
    }

	/** \fn getNumThreads()
	    \brief return the number of OpenMP threads
	 */
	inline int getNumThreads() { return num_threads_; }

	/** \fn parseArgs(int argc, char* argv[], argvs &args)
	    \brief parse user arguments and set simulation parameters
	 */
	inline void parseArgs(int argc, char* argv[], argvs &args);

	/** \fn init(argvs &args)
	    \brief initialize the data structures needed for the simulation
	 */
	inline void init(argvs &args);

	/** \fn cleanup(argvs &args)
	    \brief clean up the data structures created for the simulation
	 */
	inline void cleanup(argvs &args);

	/** \fn run(argvs &args, stats &stats)
	    \brief run the I/O simulation, calls init and cleanup as well
	 */
	inline void run(argvs &args, stats &stats);

	/** \fn run_loop(argvs &args)
	    \brief run the I/O simulation as a single, large OpenMP loop
	 */
	inline void run_loop(argvs &args, stats &stats);

	/** \fn run_task(argvs &args)
	    \brief run the I/O simulation as OpenMP tasks and dependencies
	 */
	inline void run_task(argvs &args);
};

#include "keyvalue/mpikeyvalue.ipp"
#endif
