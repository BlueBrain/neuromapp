#include <hdf5.h>
#include <mpi.h>
#include <algorithm>
#include <numeric>
#include <vector>

#include "hdf5/H5SynapseLoader.h"

#ifndef H5SYNAPSESLOADER_CLASS
#define H5SYNAPSESLOADER_CLASS


  size_t
  H5SynapsesLoader::getNumberOfSynapses( H5Dataset& dataset )
  {
    hsize_t count;

    hid_t dataspace_id = H5Dget_space( dataset.getId() );
    H5Sget_simple_extent_dims(
      dataspace_id, &count, NULL ); // get vector length from x dataset length
    H5Sclose( dataspace_id );

    return count;
  }

  H5SynapsesLoader::H5SynapsesLoader( const std::string h5file,
    const std::vector< std::string > prop_names,
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
    syn_dataset = new H5Dataset( this, "syn" );


    // read number of synapses
    total_num_syns_ = getNumberOfSynapses();
    n_SynapsesInDatasets += total_num_syns_;
    // adapt if specified
    if ( lastSyn > 0 )
      setLastSyn( lastSyn ); // should be called berfore loadNeuronLinks

      // create syn memtype
    num_compound_ = prop_names.size() + 1;


      syn_memtype_ = H5Tcreate( H5T_COMPOUND,
        sizeof( int )
          + sizeof( float ) * prop_names.size() ); // target + param values
      // target is always given
      H5Tinsert( syn_memtype_, "target", 0, H5T_NATIVE_INT ); // first entry in pool is target id
      // additionally load following columns
      for ( int i = 0; i < prop_names.size(); i++ )
        H5Tinsert( syn_memtype_, prop_names[ i ].c_str(), sizeof( int ) + i * sizeof( float ), H5T_NATIVE_FLOAT ); // following entries in pool are param values



  }

  H5SynapsesLoader::~H5SynapsesLoader()
    {

      H5Tclose( syn_memtype_ );

      delete syn_dataset;

      H5Gclose( gid_ );
      H5Fclose( file_id_ );
    }

    void H5SynapsesLoader::setLastSyn( uint64_t last )
    {
      if ( last < total_num_syns_ )
        total_num_syns_ = last;
    }

    /*
     * Returns the number of synapses (entries in syn dataset)
     */
    size_t H5SynapsesLoader::getNumberOfSynapses()
    {
      // H5Dataset cell_dataset(this,"syn");
      return getNumberOfSynapses( *syn_dataset );
    }
    /*
     * returns of file pointer reached end of file
     */
    bool H5SynapsesLoader::eof()
    {
      return total_num_syns_ <= global_offset_;
    }

    /*
     * Get num_syns synapses from datasets collectivly
     * Move file pointer to for next function call
     *
     */
    void H5SynapsesLoader::iterateOverSynapsesFromFiles( std::vector< int > & buffer )
    {
      uint64_t private_offset = fixed_num_syns_ * RANK + global_offset_;
      global_offset_ += fixed_num_syns_ * NUM_PROCESSES;

      // load only neuron parameters which are needed based on NEST internal
      // round robin fashion

      int64_t count = std::min( ( int64_t ) fixed_num_syns_,
        ( ( int64_t ) total_num_syns_ - ( int64_t ) private_offset ) );
      if ( count < 0 )
        count = 0;
      H5View dataspace_view( count, private_offset );

      hid_t dataspace_id = H5Dget_space( syn_dataset->getId() );
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
        memspace_id = H5Screate_simple( 1, dataspace_view.count, NULL );
      }
      else
      {
        H5Sselect_none( dataspace_id );
        memspace_id = H5Scopy( dataspace_id );
        H5Sselect_none( memspace_id );
      }

      buffer.resize( dataspace_view.count[ 0 ] * num_compound_ );

      // setup collective read operation
      hid_t dxpl_id_ = H5Pcreate( H5P_DATASET_XFER );
      // H5Pset_dxpl_mpio(dxpl_id_, H5FD_MPIO_COLLECTIVE);
      //H5Pset_dxpl_mpio( dxpl_id_, H5FD_MPIO_INDEPENDENT );

      H5Dread( syn_dataset->getId(),
        syn_memtype_,
        memspace_id,
        dataspace_id,
        dxpl_id_,
        &buffer.property_pool_[0] );

      H5Pclose( dxpl_id_ );

      H5Sclose( memspace_id );
      H5Sclose( dataspace_id );

      // observer variable
      n_readSynapses += dataspace_view.count[ 0 ];
    }
};

#endif
