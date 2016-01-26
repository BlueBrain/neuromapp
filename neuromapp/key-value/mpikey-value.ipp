#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include <mpi.h>

#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "key-value/mpikey-value.h"
#include "key-value/map/map_store.h"

#ifdef SKV_STORE
#include "key-value/skv/skv_store.h"
#endif


template<int h>
void KeyValueBench<h>::parseArgs(int argc, char * argv[], KeyValueArgs & args)
{
	for (int i = 1; i < argc; i++) {
		std::string param(argv[i]);

		if (param == "-b") {
			args.backend() = std::string(argv[i + 1]);
			i++;
		} else if (param == "-a") {
			args.async() = true;
		} else if (param == "-f") {
			args.flash() = true;
		} else if (param == "-uc") {
			args.usecase() = std::atoi(argv[i + 1]);
			i++;
		} else if (param == "-st") {
			args.st() = std::atof(argv[i + 1]);
			i++;
		} else if (param == "-md") {
			args.md() = std::atof(argv[i + 1]);
			i++;
		} else if (param == "-dt") {
			args.dt() = std::atof(argv[i + 1]);
			i++;
		} else if (param == "-cg") {
			args.cg() = std::atoi(argv[i + 1]);
			i++;
		} else {
			std::cout << "Ignoring invalid parameter: " << argv[i] << std::endl;
		}
	}


     #pragma omp parallel
	{
        #ifdef _OPENMP
		num_threads_ = omp_get_num_threads();
        #endif
        num_threads_ = 1;

    }

	if (rank_ == 0) {
		std::cout << "Using the following configuration:" << std::endl
				<< "    " << "Key-Value backend: " << args.backend() << std::endl
				<< "    " << (args.async() ? "Asynchronous" : "Synchronous") << " SKV API" << std::endl
				<< "    " << "Using " << (args.flash() ? "flash memory" : "disk") << " as storage" << std::endl
				<< "    " << "Use case #" << args.usecase() << " (" << args.usecase() * 25 << "% of DRAM)" << std::endl
				<< "    " << "Simulation time: " << args.st() << " ms" << std::endl
				<< "    " << "Min delay: " << args.md() << " ms" << std::endl
				<< "    " << "Delta time: " << args.dt() << " ms" << std::endl
				<< "    " << args.cg() << (args.cg() == 1 ? " cell group" : " cell groups") << " per process" << std::endl
				<< "    " << args.procs() << (args.procs() == 1 ? " MPI process" : " MPI processes") << std::endl
				<< "    " << args.threads() << (args.threads() == 1 ? " OpenMP thread" : " OpenMP threads") << std::endl
				<< std::endl;
	}
}


template<int h>
void KeyValueBench<h>::init(KeyValueArgs & args) {




     //Should create an object of type keyvalue_handle<...>, ... = skv, map, ldb
	if (args.backend() == "skv") {
#ifdef SKV_STORE
		kv_store_ = new KeyValueSkv(rank_, /* thread-safe? */ (num_threads_ > 1), "IOmapp");
#else
		std::cout << "Error: skv backend not available." << std::endl;
		exit(1);
#endif

	} else if (args.backend() == "map") {
		kv_store_ = new KeyValueMap(rank_, /* thread-safe? */ (num_threads_ > 1), "IOmapp");

	} else if (args.backend() == "ldb") {
#ifdef SKV_LDB
		if (flash) {
			kv_store_ = new SKVLDB(rank_, /* thread-safe? */ (num_threads_ > 1), "/scratch/jplanas/kv/ldb/IOmapp");
		} else {
			kv_store_ = new SKVLDB(rank_, /* thread-safe? */ (num_threads_ > 1), "./IOmapp");
		}
#else
		std::cout << "Error: leveldb backend not available." << std::endl;
		exit(1);
#endif

	} else {
		std::cout << "Error: backend " << args.backend() << " not supported." << std::endl;
		exit(1);
	}

    int num_its = args.st() / args.dt();

    voltages_size_ = (((args.usecase() * 4096) / 2.5) * 350);

    if (args.backend() == "skv") {
        // Reduce the size to avoid SKV hanging
        voltages_size_ /= 1000;
    }

    voltages_.reserve(args.cg());
    for (unsigned int i = 0; i < voltages_.capacity(); i++) {
        voltages_.push_back(std::vector<double>());
    }

    int cg_size = voltages_size_ / args.cg();
    int first_size = cg_size + (voltages_size_ % args.cg());

    voltages_[0].reserve(first_size);

    for (int i = 1; i < args.cg(); i++) {
        voltages_[i].reserve(cg_size);
    }
	float voltage = 0.1;
	float inc_v = 1.3;
	float max_v = 75.0;

	for (unsigned int i = 0; i < voltages_.size(); i++) {
		for (unsigned int v = 0; v < voltages_[i].capacity(); v++) {
			voltages_[i].push_back(voltage);
			voltage += inc_v;
			if (voltage > max_v || voltage < (-max_v)) {
				inc_v = -inc_v;
			}
		}
	}


	std::cout << "Using " << voltages_size_ << " voltages, sizeof vector is "
			<< ( voltages_size_ * sizeof(double) ) / (double) (1024 * 1024) << " MB" << std::endl;

	gids_.reserve(args.cg());
	for (unsigned int i = 0; i < gids_.capacity(); i++) {
		gids_.push_back(rank_ * num_procs_ + i+1);
	}

	ins_handles_.reserve(args.cg() * num_its);
	for (unsigned int i = 0; i < ins_handles_.capacity(); i++) {
#ifdef SKV_STORE
		ins_handles_.push_back(skv_client_cmd_ext_hdl_t());
#else
		ins_handles_.push_back(0);
#endif
	}

	rem_handles_.reserve(args.cg() * num_its);
	for (unsigned int i = 0; i < rem_handles_.capacity(); i++) {
#ifdef SKV_STORE
		rem_handles_.push_back(skv_client_cmd_ext_hdl_t());
#else
		rem_handles_.push_back(0);
#endif
	}

	MPI_Barrier(MPI_COMM_WORLD);


#ifdef SKV_STORE
	// Erase any potential previous value
	for (float st = 0; st < args.st_; st += args.md_) {
		for (float md = 0; md < args.md_; md += args.dt_) {

			int handle_idx = ( ((st/args.md_) * (args.md_/args.dt_)) + (md/args.dt_) ) * args.cg_;

			for (int cg = 0; cg < args.cg_; cg++) {
				kv_store_->remove(&gids_[cg], sizeof(int), NULL, false);
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
#endif

}

template<int h>
void KeyValueBench<h>::cleanup(KeyValueArgs & args) {
	std::cout << "Removing inserted values" << std::endl;

	//for (float st = 0; st < args.st_; st += args.md_) {
		//for (float md = 0; md < args.md_; md += dt) {
			for (int cg = 0; cg < args.cg(); cg++) {
				kv_store_->retrieve(&gids_[cg], sizeof(int), &voltages_[cg][0], voltages_[cg].size() * sizeof(double));
			}
		//}
	//}

	for (float st = 0; st < args.st(); st += args.md()) {
		for (float md = 0; md < args.md(); md += args.dt()) {

			int handle_idx = ( ((st/args.md()) * (args.md()/args.dt())) + (md/args.dt()) ) * args.cg();

			for (int cg = 0; cg < args.cg(); cg++) {
				kv_store_->remove(&gids_[cg], sizeof(int), &rem_handles_[handle_idx + cg], args.async());
			}

			for (int cg = 0; args.async() && cg < args.cg(); cg++) {
				kv_store_->wait(&rem_handles_[handle_idx + cg]);
			}

			std::cout << "Values removed for sim time " << st << ", min delay " << md << std::endl;
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);


//	delete skv_;
}

template<int h>
void KeyValueBench<h>::run(KeyValueArgs & args, KeyValueStats &stats) {

	init(args);


    

	int comp_time_us = 100 * args.usecase() * 1000;



#if _OPENMP < 201307
	run_loop(args, stats);
#else
	run_task(args);
#endif

	cleanup(args);

	stats.rank_iops_ = stats.rank_iops_ / args.dt();
	stats.rank_mbw_ = stats.rank_mbw_ / args.dt();

	std::cout << "[" << rank_ << "] I/O: " << stats.rank_iops_ << " IOPS; BW: " << stats.rank_mbw_ << " MB/s" << std::endl;

    MPI_Allreduce( &stats.rank_iops_, &stats.mean_iops_, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
    MPI_Allreduce( &stats.rank_mbw_, &stats.mean_mbw_, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

    // Unit conversion
    // IOPS --> KIOPS
    stats.mean_iops_ = stats.mean_iops_ / 1000;
    // MB/s --> GB/s
    stats.mean_mbw_ = stats.mean_mbw_ / 1024;

    // Mean values per node
    //stats.mean_iops_ = stats.mean_iops_ / num_procs_;
    //stats.mean_mbw_ = stats.mean_mbw_ / num_procs_;

    MPI_Barrier(MPI_COMM_WORLD);


}



template<int h>
void KeyValueBench<h>::run_loop(KeyValueArgs & args, KeyValueStats & stats) {
#if 1
	// OPENMP lower than 4.0, no task deps support

	int comp_time_us = 100 * args.usecase() * 1000;

		for (float st = 0; st < args.st(); st += args.md()) {
			for (float md = 0; md < args.md(); md += args.dt()) {

				usleep(comp_time_us);

				int reqs = args.cg();
				unsigned int bytes = voltages_size_ * sizeof(double);


				int handle_idx = ( ((st/args.md()) * (args.md()/args.dt())) + (md/args.dt()) ) * args.cg();

				double start = MPI_Wtime();

				//#pragma omp parallel for reduction (+: reqs, bytes) shared(voltages_, gids, ts, cg, ins_handles, async)
             	#pragma omp parallel for shared(voltages_, gids_, st, /*args.cg_,*/ ins_handles_, handle_idx/*, args.async_*/) //num_threads(omp_threads)
				for (int cg = 0; cg < args.cg(); cg++) {
					kv_store_->insert(&gids_[cg], sizeof(int), &voltages_[cg][0], voltages_[cg].size() * sizeof(double),
                                      &ins_handles_[handle_idx + cg], args.async());
				}

				for (int cg = 0; args.async() && cg < args.cg(); cg++) {
					kv_store_->wait(&ins_handles_[handle_idx + cg]);
				}

				double end = MPI_Wtime();

				double time = end - start; // seconds
				double iops = reqs / time; // I/O ops/s
				double mbw = (bytes / time) / (1024 * 1024); // MB/s

				// Add the results for every iteration
				stats.rank_iops_ += iops;
				stats.rank_mbw_ += mbw;

				double g_iops = 0, g_mbw = 0;

				MPI_Allreduce( &iops, &g_iops, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
				MPI_Allreduce( &mbw, &g_mbw, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

				MPI_Barrier(MPI_COMM_WORLD);

				if (rank_ == 0) {
					std::cout << "Values inserted for sim time " << st << ", min delay " << md << ", global performance:" << std::endl
							<< "  Time: " << time << " s. (rank 0)" << std::endl
							<< "  I/O: " << iops << " IOPS" << std::endl
							<< "  BW: " << mbw << " MB/s" << std::endl;
				}
			}
		}
#endif

}


template<int h>
void KeyValueBench<h>::run_task(KeyValueArgs & args) {
#if 0
	// OPENMP 4.0 or later, support for task deps

		double start, end, start_io, end_io;
		int sleep_time = 0;
		int adjust = 1000 * uc / omp_threads;
		int reqs = num_its * args.cg_;
		unsigned int bytes = num_its * (num_voltages * sizeof(double));


		do {

		/***********************************************************************************/
		/******************** Compute base line computation without I/O ********************/
		/***********************************************************************************/

		start = MPI_Wtime();


		for (float st = 0; st < args.st_; st += args.md_) {
			for (float md = 0; md < args.md_; md += dt) {
	#pragma omp parallel if(0)
				{
	#pragma omp single nowait
					{
						for (int cg = 0; cg < args.cg_; cg++) {
							double * dep1 = &voltages_[cg][0];
							double * dep2 = &voltages_[cg][1];

							sleep_time += (int) ((0.45 * comp_time_us) / omp_threads) + adjust;
	#pragma omp task depend(inout:dep1)
							{
								// First computation - 45%
								usleep((int) ((0.45 * comp_time_us) / omp_threads) + adjust);
							}

							sleep_time += (int) ((0.05 * comp_time_us) / omp_threads) + adjust;
	#pragma omp task depend(inout:dep1, dep2)
							{
								// Linear algebra - 5%
								usleep((int) ((0.05 * comp_time_us) / omp_threads) + adjust);
							}

							sleep_time += (int) ((0.38 * comp_time_us) / omp_threads) + adjust;
	#pragma omp task depend(inout:dep1)
							{
								// Second computation - 38%
								usleep((int) ((0.38 * comp_time_us) / omp_threads) + adjust);
							}
						}

					} // omp single
				} // omp parallel

			}
	#pragma omp barrier

			sleep_time += (int) ((0.05 * comp_time_us)) + adjust;
	//#pragma omp task depend(inout:dep1)
	//		{
				// Spike exchange - 5%
				usleep((int) (0.05 * comp_time_us) + adjust);
	//		}
		}

		end = MPI_Wtime();

		/***********************************************************************************/


		int handle_idx = 0;
		int prev_handle_idx = -1;

		start_io = MPI_Wtime();

		for (float st = 0; st < args.st_; st += args.md_) {
			for (float md = 0; md < args.md_; md += dt) {
	#pragma omp parallel
				{
	#pragma omp single nowait
					{
						handle_idx = ( ((st/args.md_) * (args.md_/dt)) + (md/dt) ) * args.cg_;

						for (int cg = 0; cg < args.cg_; cg++) {

							double * dep1 = &voltages_[cg][0];
							double * dep2 = &voltages_[cg][1];

	#pragma omp task depend(inout:dep1)
							{
								// First computation - 45%
								usleep((int) ((0.45 * comp_time_us) / omp_threads) + adjust);
							}

	#pragma omp task depend(inout:dep1, dep2)
							{
								// Wait for I/O from previous iteration
								// FIXME: This should be implemented in a cleaner way...
								if (async) {
									skv->wait(&ins_handles[prev_handle_idx + cg]);
								}

								// Linear algebra - 5%
								usleep((int) ((0.05 * comp_time_us) / omp_threads) + adjust);
							}

	#pragma omp task depend(inout:dep2)
							{
								// I/O
								skv->insert(&gids[cg], sizeof(int), &voltages_[cg][0], voltages_[cg].size() * sizeof(double), &ins_handles[handle_idx + cg], async);
							}

	#pragma omp task depend(inout:dep1)
							{
								// Second computation - 38%
								usleep((int) ((0.38 * comp_time_us) / omp_threads) + adjust);
							}

	#if 0
							// Not optimal doing it here, as we could still run the first computation task from next iteration before this
							if (async) {
	#pragma omp task depend(inout:dep2)
								{
									skv->wait(&ins_handles[handle_idx + cg]);
								}
							}
	#endif
						}

						prev_handle_idx = handle_idx;

					} // omp single
				} // omp parallel

			}

	#pragma omp barrier

			//#pragma omp task depend(inout:dep1)
			//		{
			// Spike exchange - 5%
			usleep((int) (0.05 * comp_time_us) + adjust);
			//		}
		}

		end_io = MPI_Wtime();

		if ((end_io - start_io) - (end - start) < 0) std::cout << "Negative time, computing again..." << std::endl;

		} while ((end_io - start_io) - (end - start) < 0);

		double time = (end_io - start_io) - (end - start); // seconds
		double iops = reqs / time; // I/O ops/s
		double mbw = (bytes / time) / (1024 * 1024); // MB/s

		// Add the results for every iteration
		rank_iops += iops;
		rank_mbw += mbw;

		double g_iops = 0, g_mbw = 0;

		MPI_Allreduce( &iops, &g_iops, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
		MPI_Allreduce( &mbw, &g_mbw, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

		MPI_Barrier(MPI_COMM_WORLD);

		if (mpi_rank == 0) {
			//std::cout << "Values inserted for sim time " << st << ", min delay " << md << ", global performance:" << std::endl
			std::cout << "Values inserted, global performance:" << std::endl
					<< "MPI_Wtime resolution: " << MPI_Wtick() << std::endl
					<< "  Theoretical computation time is: " << 0.93 * num_its * (comp_time_us) << " us." << std::endl
					<< "  Computed sleep time is: " << sleep_time << " us." << std::endl
					<< "  Computation time reference without I/O: " << (end - start) << " s. (rank 0)" << std::endl
					<< "  Computation time with I/O: " << (end_io - start_io) << " s. (rank 0)" << std::endl
					<< "  Time: " << time << " s. (rank 0)" << std::endl
					<< "  I/O: " << iops << " IOPS" << std::endl
					<< "  BW: " << mbw << " MB/s" << std::endl;
		}

#endif

}




