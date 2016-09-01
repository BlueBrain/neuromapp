/*
 * Neuromapp - args.cpp, Copyright (c), 2015,
 * Till Schumann - Swiss Federal Institute of technology in Lausanne,
 * till.schumann@epfl.ch,
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
 * GNU General Public License for more details. See
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/nest/synapse.cpp
 *  Test on the nest synapse module
 */

#define BOOST_TEST_MODULE nesth5import
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <algorithm>

#include <mpi.h>

#include "utils/error.h"

#include "test/tools/mpi_helper.h"

#include "nest/h5import/H5Synapses.h"
#include "nest/h5import/h5reader.h"
#include "nest/h5import/NESTNodeSynapse.h"
#include "nest/h5import/kernels.h"
#include "hdf5/data/helper.h"


BOOST_AUTO_TEST_CASE(nest_h5import_kernels)
{
    h5import::TokenArray cof;
    cof.push_back(1.);
    cof.push_back(2.);
    h5import::kernel_add< float > kadd(cof);
    h5import::kernel_multi< float > kmulti(cof);

    std::vector< float > start(2,0);
    std::vector< float >* values = &start;

    values = kadd(values->begin(), values->end());
    BOOST_CHECK_CLOSE( (*values)[0], 1., 0.000001 );
    BOOST_CHECK_CLOSE( (*values)[1], 2., 0.000001 );

    values = kmulti(values->begin(), values->end());
    BOOST_CHECK_CLOSE( (*values)[0], 1., 0.000001 );
    BOOST_CHECK_CLOSE( (*values)[1], 4., 0.000001 );

    h5import::kernel_combi< float > kcombi;
    kcombi.push_back< h5import::kernel_add<float> >(cof);
    kcombi.push_back< h5import::kernel_multi<float> >(cof);

    values = kcombi(values->begin(), values->end());
    BOOST_CHECK_CLOSE( (*values)[0], 2., 0.000001 );
    BOOST_CHECK_CLOSE( (*values)[1], 12., 0.000001 );
}

BOOST_AUTO_TEST_CASE(nest_h5import_open_file)
{
    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    unsigned long long fixed_num_syns=524288;
 
    std::vector< std::string > datasets;
    h5import::h5reader loader(hdf5::testdata_compound(),
       datasets,
      fixed_num_syns);

    size_t num_syns = loader.size();

    BOOST_CHECK_EQUAL(num_syns, 126);
}

BOOST_AUTO_TEST_CASE(nest_h5import_open_colums)
{
    uint64_t transferSize=20;

    std::vector< std::string > datasets;
    datasets.push_back("delay");
    datasets.push_back("TauRec");

    h5import::h5reader loader(hdf5::testdata_compound(),
                datasets,
                transferSize);

    h5import::NESTSynapseList synapses;
    synapses.set_properties(datasets);

    h5import::h5reader::h5view view;

    loader.readblock( synapses, view );
    BOOST_CHECK_EQUAL( synapses.size(), transferSize );
    BOOST_CHECK_EQUAL( view.count[0], transferSize );
}


BOOST_AUTO_TEST_CASE(nest_h5import_parameter_values)
{
    h5import::kernel_env env;

    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    uint64_t fixed_num_syns=20;
    
    std::vector< std::string > datasets;
    datasets.push_back("delay");
    datasets.push_back("weight");
    datasets.push_back("U0");
    datasets.push_back("TauRec");
    datasets.push_back("TauFac");
      
    h5import::h5reader loader(hdf5::testdata_compound(),
            datasets,
      fixed_num_syns);
    const uint64_t dataset_size = loader.size();
    BOOST_CHECK_EQUAL(126, dataset_size);

    uint64_t read_size = 0;
    uint64_t accu_targets = 0;

    const int entries_per_row = datasets.size();
    while( !loader.eof() ) {
        h5import::NESTSynapseList synapses;
        h5import::h5reader::h5view view;
        synapses.set_properties(datasets);

        loader.readblock( synapses, view );
        BOOST_CHECK_MESSAGE(synapses.size() <= fixed_num_syns, "unvalid buffer size");

        for (int j=0; j<synapses.size(); j++) {
           const int target = synapses[j].target_neuron_;
           accu_targets += target;
           const float delay = synapses[j].params_[0];
           const float weight = synapses[j].params_[1];
           const float U0 = synapses[j].params_[2];
           const float TauRec = synapses[j].params_[3];
           const float TauFac = synapses[j].params_[4];

           BOOST_CHECK_CLOSE(target, delay+126, 0.000001);
           BOOST_CHECK_CLOSE(target, weight-126, 0.000001);
           BOOST_CHECK_CLOSE(target, U0+252, 0.000001);
           BOOST_CHECK_CLOSE(target, TauRec+378, 0.000001);
           BOOST_CHECK_CLOSE(target, TauFac+504, 0.000001);
        }
        read_size += synapses.size();
    }

    uint64_t total_read_size;
    MPI_Reduce(&read_size, &total_read_size, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    uint64_t total_accu_targets;
    MPI_Reduce(&accu_targets, &total_accu_targets, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank==0) {
        BOOST_CHECK_EQUAL(total_read_size, 126);
        BOOST_CHECK_EQUAL(total_accu_targets, 71379);
    }
}

BOOST_AUTO_TEST_CASE(nest_h5import_import)
{
    const int nthreads = 2;
    const int ncells = 1000;

    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    omp_set_num_threads(nthreads);

    //setup fake nest kernel environment
    h5import::kernel_env kenv;
    environment::nestdistribution neuro_mpi_dist(num_processes, rank, ncells);
    h5import::kernel().set_mpi_dist(&neuro_mpi_dist);

    std::vector<environment::nestdistribution*> neuro_vp_dists;
    for (int thrd=0; thrd<nthreads; thrd++) {
        neuro_vp_dists.push_back(new environment::nestdistribution(nthreads, thrd, &neuro_mpi_dist));
        h5import::kernel().set_vp_dist (thrd, neuro_vp_dists[thrd] );
    }

    //setup h5import module
    h5import::H5Synapses h5synapses;
    h5synapses.set_filename(hdf5::testdata_compound());
    std::vector< std::string > props;
    props.push_back("delay");
    props.push_back("weight");
    h5synapses.set_properties(props);
    h5import::GIDCollection gids;
    h5synapses.set_mapping(gids);
    h5synapses.set_transfersize(10);

    //run h5 import module
    h5synapses.import();

    uint64_t num_connections=0;
    double accu_syn_props=0.0;
    for (int thrd=0; thrd<nthreads; thrd++) {
        if (h5import::kernel().connection_manager.num_connections.size() > thrd)
            num_connections += h5import::kernel().connection_manager.num_connections[thrd];
        if (h5import::kernel().connection_manager.sum_values.size() > thrd)
            accu_syn_props += h5import::kernel().connection_manager.sum_values[thrd];
    }
    BOOST_CHECK_EQUAL(num_connections, 106);
    BOOST_CHECK_CLOSE(accu_syn_props, 121058, 0.00001);

    //get ride of thread distributions
    for (int i=0; i<nthreads; i++)
        delete neuro_vp_dists[i];
}


