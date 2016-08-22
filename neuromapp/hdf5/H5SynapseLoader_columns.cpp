/*
 * H5SynapseLoader_columns.cpp
 *
 *  Created on: Aug 16, 2016
 *      Author: schumann
 */

#include "hdf5/H5SynapseLoader_columns.h"

H5SynapsesLoader::H5SynapsesLoader( const std::string h5file,
    const std::vector< std::string > datasets,
    uint64_t& n_readSynapses,
    uint64_t& n_SynapsesInDatasets,
    uint64_t fixed_num_syns,
    uint64_t lastSyn)
    : global_offset_( 0 )
    , n_readSynapses( n_readSynapses )
    , n_SynapsesInDatasets( n_SynapsesInDatasets )
    , fixed_num_syns_( fixed_num_syns )
{
    assert( fixed_num_syns_ > 0 );

    MPI_Comm_size( MPI_COMM_WORLD, &NUM_PROCESSES );
    MPI_Comm_rank( MPI_COMM_WORLD, &RANK );
    // open hdf5 in parallel mode
    hid_t fapl_id = H5Pcreate( H5P_FILE_ACCESS );
    //H5Pset_fapl_mpio( fapl_id, MPI_COMM_WORLD, MPI_INFO_NULL );
    file_id_ = H5Fopen( h5file.c_str(), H5F_ACC_RDONLY, fapl_id );
    H5Pclose( fapl_id );

    gid_ = H5Gopen( file_id_, "/", H5P_DEFAULT );


    for (int i=0; i<datasets.size(); i++)
        syn_datasets.push_back(new H5Dataset( this, datasets[i].c_str() ));


    // read number of synapses
    total_num_syns_ = getNumberOfSynapses();
    n_SynapsesInDatasets += total_num_syns_;
    // adapt if specified
    if ( lastSyn > 0 )
      setLastSyn( lastSyn ); // should be called berfore loadNeuronLinks
}

H5SynapsesLoader::~H5SynapsesLoader()
{
  for (int i=0; i<syn_datasets.size(); i++)
      delete syn_datasets[i];

  H5Gclose( gid_ );
  H5Fclose( file_id_ );
}

void H5SynapsesLoader::setLastSyn( uint64_t last )
{
  if ( last < total_num_syns_ )
    total_num_syns_ = last;
}

size_t
H5SynapsesLoader::getNumberOfSynapses( H5SynapsesLoader::H5Dataset& dataset )
{
  hsize_t count;

  hid_t dataspace_id = H5Dget_space( dataset.getId() );
  H5Sget_simple_extent_dims(
    dataspace_id, &count, NULL ); // get vector length from x dataset length
  H5Sclose( dataspace_id );

  return count;
}

size_t H5SynapsesLoader::getNumberOfSynapses()
{
  return getNumberOfSynapses( *syn_datasets[0] );
}

bool H5SynapsesLoader::eof()
{
  return total_num_syns_ <= global_offset_;
}


void H5SynapsesLoader::iterateOverSynapsesFromFiles( std::vector<int> & buffer )
{
  //
  uint64_t local_offset = fixed_num_syns_ * RANK + global_offset_;
  global_offset_ += fixed_num_syns_ * NUM_PROCESSES;

  int64_t count = std::min( ( int64_t ) fixed_num_syns_,
    ( ( int64_t ) total_num_syns_ - ( int64_t ) local_offset ) );
  if ( count < 0 )
    count = 0;
  H5View dataspace_view( count, local_offset );

  buffer.resize( dataspace_view.count[ 0 ] * syn_datasets.size() );

  for (int i=0; i<syn_datasets.size(); i++) {
      H5View memspace_view( count, i, syn_datasets.size());

      hid_t dataspace_id = H5Dget_space( syn_datasets[i]->getId() );
      hid_t memspace_id;

      // be careful if there are no synapses to load
      if ( dataspace_view.count[ 0 ] > 0 )
      {
        H5Sselect_hyperslab( dataspace_id,
          H5S_SELECT_SET,
          dataspace_view.offset,
          dataspace_view.stride,
          dataspace_view.count,
          dataspace_view.block );

        hsize_t pool_size = buffer.size();
        memspace_id = H5Screate_simple( 1, &pool_size, NULL );
        H5Sselect_hyperslab( memspace_id,
          H5S_SELECT_SET,
          memspace_view.offset,
          memspace_view.stride,
          memspace_view.count,
          memspace_view.block );
      }
      else
      {
        H5Sselect_none( dataspace_id );
        memspace_id = H5Scopy( dataspace_id );
        H5Sselect_none( memspace_id );
      }

      // setup collective read operation
      hid_t dxpl_id_ = H5Pcreate( H5P_DATASET_XFER );
      //H5Pset_dxpl_mpio(dxpl_id_, H5FD_MPIO_COLLECTIVE);
      //H5Pset_dxpl_mpio( dxpl_id_, H5FD_MPIO_INDEPENDENT );

      hid_t mem_type_id;
      if (i==0)
          mem_type_id = H5T_NATIVE_INT;
      else
          mem_type_id = H5T_NATIVE_FLOAT;

      H5Dread( syn_datasets[i]->getId(),
        mem_type_id,
        memspace_id,
        dataspace_id,
        dxpl_id_,
        &buffer[0] );

      H5Pclose( dxpl_id_ );

      H5Sclose( memspace_id );
      H5Sclose( dataspace_id );
  }

  // observer variable
  n_readSynapses += dataspace_view.count[ 0 ];
}


