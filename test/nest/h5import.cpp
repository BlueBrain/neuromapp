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
#include "nest/h5import/SynapseList.h"
#include "nest/h5import/kernels.h"
#include "hdf5/data/helper.h"


BOOST_AUTO_TEST_CASE(nest_h5import_kernels)
{
    //setup fake nest kernel environment
    h5import::kernel_env kenv( 100, 1, 1, 4 );
	
    h5import::TokenArray cof;
    cof.push_back( 1. );
    cof.push_back( 2. );
    h5import::kernel_add< float > kadd( cof );
    h5import::kernel_multi< float > kmulti( cof );

    std::vector< float > start(2,0);
    std::vector< float >* values = &start;
	
	BOOST_CHECK( true );

    values = kadd( values->begin(), values->end() );
    BOOST_CHECK_CLOSE( (*values)[0], 1., 0.000001 );
    BOOST_CHECK_CLOSE( (*values)[1], 2., 0.000001 );

    values = kmulti( values->begin(), values->end() );
    BOOST_CHECK_CLOSE( (*values)[0], 1., 0.000001 );
    BOOST_CHECK_CLOSE( (*values)[1], 4., 0.000001 );

    h5import::kernel_combi< float > kcombi;
    kcombi.push_back< h5import::kernel_add<float> >( cof );
    kcombi.push_back< h5import::kernel_multi<float> >( cof );

    values = kcombi( values->begin(), values->end() );
    BOOST_CHECK_CLOSE( ( *values )[0], 2., 0.000001 );
    BOOST_CHECK_CLOSE( ( *values )[1], 12., 0.000001 );
}

BOOST_AUTO_TEST_CASE(nest_h5import_open_file)
{
    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    unsigned long long fixed_num_syns=524288;
 
    std::vector< std::string > datasets;
    h5import::h5reader loader( hdf5::testdata_compound(),
       datasets,
      fixed_num_syns );

    size_t num_syns = loader.size();

    BOOST_CHECK_EQUAL( num_syns, 126 );
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

    h5import::SynapseList synapses( datasets.size() );

    h5import::h5reader::h5view view;

    loader.readblock( synapses, view );
    BOOST_CHECK_EQUAL( synapses.size(), transferSize );
    BOOST_CHECK_EQUAL( view.count[0], transferSize );
}


BOOST_AUTO_TEST_CASE(nest_h5import_parameter_values)
{
    //h5import::kernel_env env(  );

    int num_processes;
    int rank;
    MPI_Comm_size( MPI_COMM_WORLD, &num_processes );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    
    uint64_t fixed_num_syns=20;
    
    std::vector< std::string > datasets;
    datasets.push_back( "delay" );
    datasets.push_back( "weight" );
    datasets.push_back( "U0" );
    datasets.push_back( "TauRec" );
    datasets.push_back( "TauFac" );
      
    h5import::h5reader loader(hdf5::testdata_compound(),
            datasets,
      fixed_num_syns);
    const uint64_t dataset_size = loader.size();
    BOOST_CHECK_EQUAL(126, dataset_size);

    uint64_t read_size = 0;
    uint64_t accu_targets = 0;

    const int entries_per_row = datasets.size();
    while( !loader.eof() ) {
        h5import::SynapseList synapses( datasets.size() ) ;
        h5import::h5reader::h5view view;

        loader.readblock( synapses, view );
        BOOST_CHECK_MESSAGE(synapses.size() <= fixed_num_syns, "unvalid buffer size");

        for ( int j=0; j<synapses.size(); j++ ) {
           const int target = synapses[j].target_neuron_;
           accu_targets += target;
           const float delay = synapses[j].params_[0];
           const float weight = synapses[j].params_[1];
           const float U0 = synapses[j].params_[2];
           const float TauRec = synapses[j].params_[3];
           const float TauFac = synapses[j].params_[4];

           BOOST_CHECK_CLOSE( target, delay+126, 0.000001 );
           BOOST_CHECK_CLOSE( target, weight-126, 0.000001 );
           BOOST_CHECK_CLOSE( target, U0+252, 0.000001 );
           BOOST_CHECK_CLOSE( target, TauRec+378, 0.000001 );
           BOOST_CHECK_CLOSE( target, TauFac+504, 0.000001 );
        }
        read_size += synapses.size();
    }

    uint64_t total_read_size;
    MPI_Reduce( &read_size, &total_read_size, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD );
    uint64_t total_accu_targets;
    MPI_Reduce( &accu_targets, &total_accu_targets, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD );
    if (rank==0) {
        BOOST_CHECK_EQUAL( total_read_size, 126 );
        BOOST_CHECK_EQUAL( total_accu_targets, 71379 );
    }
}

BOOST_AUTO_TEST_CASE(nset_h5import_nestnodesynapse)
{
    h5import::GIDCollection gids1;
    BOOST_CHECK_EQUAL( gids1[ 13 ], 13 );
    h5import::GIDCollection gids2( 4, 100 );
    BOOST_CHECK_EQUAL( gids2[ 13 ], 17 );
    
    h5import::mpi_buffer< int > mb1( 56, true );
    BOOST_CHECK_EQUAL( mb1.size(), 56 );
    mb1.push_back( 3 );
    mb1.push_back( 5 );
    BOOST_CHECK_EQUAL( mb1.size(), 58 );
    mb1[6] = 24;
    mb1[7] = 9;
    BOOST_CHECK_EQUAL( mb1[7], 9 );
    mb1[0] = 10;
    BOOST_CHECK_EQUAL( mb1.pop_front(), 10 );
    BOOST_CHECK_EQUAL( mb1.size(), 57 );
    BOOST_CHECK_EQUAL( mb1[6], 9 );
    
    mb1.clear();
    BOOST_CHECK_EQUAL( mb1.size(), 0 );
    mb1.push_back( 67 );
    BOOST_CHECK_EQUAL( mb1.size(), 1 );
    BOOST_CHECK_EQUAL( mb1.pop_front(), 67 );
    BOOST_CHECK_EQUAL( mb1.size(), 0 );
    
    h5import::mpi_buffer< int > mb2( 56, false );
    BOOST_CHECK_EQUAL( mb2.size(), 0 );
    mb2.push_back( 78 );
    BOOST_CHECK_EQUAL( mb2.size(), 1 );
    BOOST_CHECK_EQUAL( mb2.pop_front(), 78 );
    BOOST_CHECK_EQUAL( mb2.size(), 0 );
    
    uint32_t neuron = 213;
    uint32_t node = 432;
    uint32_t pool[3];
    pool[0] = 65;
    float tmp = 1.0;
    pool[1] = *reinterpret_cast< uint32_t* >( &tmp ); tmp++;
    pool[2] = *reinterpret_cast< uint32_t* >( &tmp );
    h5import::SynapseRef ref( neuron, node, 2, reinterpret_cast< char* >( pool ) );
    
    BOOST_CHECK_EQUAL( ref.source_neuron_, 213 );
    BOOST_CHECK_EQUAL( ref.node_id_, 432 );
    BOOST_CHECK_EQUAL( ref.target_neuron_, 65 );
    BOOST_CHECK_CLOSE( *( ref.params_.begin() ), 1., 0.000001 );
    BOOST_CHECK_CLOSE( *( ref.params_.begin()+1 ), 2., 0.000001 );
    BOOST_CHECK_EQUAL( ref.params_.end()-ref.params_.begin(), 2 );
    BOOST_CHECK_CLOSE( ref.params_[ 0 ], 1., 0.000001 );
    BOOST_CHECK_CLOSE( ref.params_[ 1 ], 2., 0.000001 );
    
    h5import::mpi_buffer< int > mb3( 100, true );
    for ( int i=0; i<100; i++ )
        mb3[i] = 0;
    int n = ref.serialize( mb3, 5 );
    
    BOOST_CHECK_EQUAL( n, 3+2 );
    for ( int i=0 ;i<5; i++ )
        BOOST_CHECK_EQUAL( mb3[ i ], 0 );
    for ( int i=5 ;i<5+n; i++ )
        BOOST_CHECK( mb3[ i ] != 0 );
    for ( int i=5+n;i<100; i++ )
        BOOST_CHECK_EQUAL( mb3[i], 0 );
    
    //how may bytes where changed
    BOOST_CHECK_EQUAL( n, 5 );
    
    ref.source_neuron_ = 0;
    ref.node_id_ = 0;
    ref.target_neuron_ = 0;
    ref.params_[0] = 0.;
    ref.params_[1] = 0.;
    
    BOOST_CHECK_EQUAL( ref.source_neuron_, 0 );
    BOOST_CHECK_EQUAL( ref.node_id_, 0 );
    BOOST_CHECK_EQUAL( ref.target_neuron_, 0 );
    BOOST_CHECK_CLOSE( ref.params_[0], 0., 0.000001 );
    BOOST_CHECK_CLOSE( ref.params_[1], 0., 0.000001 );
    
    ref.deserialize( mb3, 5 );
    
    BOOST_CHECK_EQUAL( ref.source_neuron_, 213 );
    BOOST_CHECK_EQUAL( ref.node_id_, 432 );
    BOOST_CHECK_EQUAL( ref.target_neuron_, 65 );
    BOOST_CHECK_CLOSE( *( ref.params_.begin() ), 1., 0.000001 );
    BOOST_CHECK_CLOSE( *( ref.params_.begin()+1 ), 2., 0.000001 );
    BOOST_CHECK_EQUAL( ref.params_.end()-ref.params_.begin(), 2 );
    BOOST_CHECK_CLOSE( ref.params_[0], 1., 0.000001 );
    BOOST_CHECK_CLOSE( ref.params_[1], 2., 0.000001 );
    
    uint32_t neuron2, node2, pool2[3];
    h5import::SynapseRef ref2( neuron2, node2, 2, reinterpret_cast< char* >( pool2 ) );
    
    ref2 = ref;
  
    BOOST_CHECK_EQUAL( ref2.source_neuron_, 213 );
    BOOST_CHECK_EQUAL( ref2.node_id_, 432 );
    BOOST_CHECK_EQUAL( ref2.target_neuron_, 65 );
    BOOST_CHECK_CLOSE( *( ref2.params_.begin() ), 1., 0.000001 );
    BOOST_CHECK_CLOSE( *( ref2.params_.begin()+1 ), 2., 0.000001 );
    BOOST_CHECK_EQUAL( ref2.params_.end()-ref2.params_.begin(), 2 );
    BOOST_CHECK_CLOSE( ref2.params_[0], 1., 0.000001 );
    BOOST_CHECK_CLOSE( ref2.params_[1], 2., 0.000001 );
    
    h5import::SynapseList syns( 2 );
    syns.resize(2);
    BOOST_CHECK_EQUAL( syns.size(), 2 );
    BOOST_CHECK_EQUAL( syns.sizeof_pool_entry(), sizeof( ref.target_neuron_ )
                                                 + sizeof( ref.params_[0] )
                                                 + sizeof( ref.params_[1] ) );
                      
    BOOST_CHECK_EQUAL( syns.sizeof_entry(), sizeof( ref.source_neuron_ )
                                            + sizeof( ref.node_id_ )
                                            + sizeof( ref.target_neuron_ )
                                            + sizeof( ref.params_[0] )
                                            + sizeof( ref.params_[1] ) );
    syns[1] = ref2;
    BOOST_CHECK_EQUAL( syns[1].source_neuron_, 213 );
    BOOST_CHECK_EQUAL( syns[1].node_id_, 432 );
    BOOST_CHECK_EQUAL( syns[1].target_neuron_, 65 );
    BOOST_CHECK_CLOSE( *(syns[1].params_.begin()), 1., 0.000001 );
    BOOST_CHECK_CLOSE( *(syns[1].params_.begin()+1), 2., 0.000001 );
    BOOST_CHECK_EQUAL( syns[1].params_.end()-syns[1].params_.begin(), 2 );
    BOOST_CHECK_CLOSE( syns[1].params_[0], 1., 0.000001 );
    BOOST_CHECK_CLOSE( syns[1].params_[1], 2., 0.000001 );


    h5import::SynapseRef tmp_syn = syns[1];
    BOOST_CHECK_EQUAL( &(syns[1].source_neuron_), &(tmp_syn.source_neuron_) );
    BOOST_CHECK_EQUAL( &(syns[1].node_id_), &(tmp_syn.node_id_) );
    BOOST_CHECK_EQUAL( &(syns[1].target_neuron_), &(tmp_syn.target_neuron_) );
    BOOST_CHECK_EQUAL( &(syns[1].params_[0]), &(tmp_syn.params_[0]) );
    BOOST_CHECK_EQUAL( &(syns[1].params_[1]), &(tmp_syn.params_[1]) );

    syns.clear();
    BOOST_CHECK_EQUAL( syns.size(), 0 );
}

BOOST_AUTO_TEST_CASE( nest_h5import_kernel )
{
    int ncells = 1234;
    int nthreads = 4;
    int rank = 3;
    int size = 45;
    h5import::kernel_env kenv( ncells, nthreads, rank, size );
    
    BOOST_CHECK_EQUAL( h5import::kernel().mpi_manager.get_rank(), rank );
    BOOST_CHECK_EQUAL( h5import::kernel().mpi_manager.get_num_processes(), size );
    
    BOOST_CHECK_EQUAL( h5import::kernel().node_manager.size(), ncells );
    //BOOST_CHECK_EQUAL( h5import::kernel().node_manager.is_local_gid( 3 ), false );
    //BOOST_CHECK_EQUAL( h5import::kernel().node_manager.is_local_gid( 183 ), true );
    
    BOOST_CHECK_EQUAL( h5import::kernel().vp_manager.get_num_threads(), nthreads );
    BOOST_CHECK_EQUAL( h5import::kernel().vp_manager.get_thread_id(), 0 );
    
    BOOST_CHECK_EQUAL( h5import::kernel().connection_manager.num_connections.size(), nthreads );
    BOOST_CHECK_EQUAL( h5import::kernel().connection_manager.sum_values.size(), nthreads );
    
    for ( int thrd=0; thrd<nthreads; thrd++ ) {
        BOOST_CHECK_EQUAL( h5import::kernel().connection_manager.num_connections[ thrd ], 0 );
        BOOST_CHECK_CLOSE( h5import::kernel().connection_manager.sum_values[ thrd ], 0., 0.00001 );
    }
    
    std::vector< double > v(3, 1);
    h5import::kernel().connection_manager.connect( 1, 4, v );
    h5import::kernel().connection_manager.connect( 2, 3, v );
    
    int num_cons = 0;
    double accu_params = 0.;
    for ( int thrd=0; thrd<nthreads; thrd++ ) {
        num_cons += h5import::kernel().connection_manager.num_connections[ thrd ];
        accu_params += h5import::kernel().connection_manager.sum_values[ thrd ];
    }
    BOOST_CHECK_EQUAL( num_cons, 2 );
    BOOST_CHECK_CLOSE( accu_params, 6., 0.0000001 );
    
}

BOOST_AUTO_TEST_CASE(nest_h5import_import)
{
    int nthreads = 1;
    const int ncells = 1000;

    int num_processes;
    int rank;
    MPI_Comm_size( MPI_COMM_WORLD, &num_processes );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    
#ifdef _OPENMP
    omp_set_num_threads(nthreads);
#else
	nthreads = 1;
#endif

    //setup fake nest kernel environment
    h5import::kernel_env kenv( ncells, nthreads, rank, num_processes );

    //setup h5import module
    h5import::H5Synapses h5synapses;
    h5synapses.set_filename( hdf5::testdata_compound() );
    std::vector< std::string > props;
    props.push_back( "delay" );
    props.push_back( "weight" );
    h5synapses.set_parameters( props );
    h5import::GIDCollection gids;
    h5synapses.set_mapping( gids );
    h5synapses.set_transfersize( 10 );

    //run h5 import module
    h5synapses.import();

    //gather test values
    int num_connections=0;
    double accu_syn_props=0.0;
    for (int thrd=0; thrd<nthreads; thrd++) {
        if ( h5import::kernel().connection_manager.num_connections.size() > thrd )
            num_connections += h5import::kernel().connection_manager.num_connections[thrd];
        if ( h5import::kernel().connection_manager.sum_values.size() > thrd )
            accu_syn_props += h5import::kernel().connection_manager.sum_values[ thrd ];
    }
	
    int total_num_connections;
    double total_accu_syn_props;
	MPI_Reduce(&num_connections, &total_num_connections, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&accu_syn_props, &total_accu_syn_props, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0) {
    	BOOST_CHECK_EQUAL( total_num_connections, 126 );
    	BOOST_CHECK_CLOSE( total_accu_syn_props, 142758., 0.00001 );
	}
}
                      



