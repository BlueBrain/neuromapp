#include <hdf5.h>
#include <mpi.h>
#include <algorithm>
#include <numeric>
#include <vector>

#ifndef H5SYNAPSESLOADER_CLASS
#define H5SYNAPSESLOADER_CLASS

struct NeuronLink
{
  int id;
  int syn_n;
  uint64_t syn_ptr;
};

struct H5View
{
  hsize_t offset[ 1 ];
  hsize_t stride[ 1 ];
  hsize_t count[ 1 ];
  hsize_t block[ 1 ];

  H5View( hsize_t icount,
    hsize_t ioffset = 0,
    hsize_t istride = 1,
    hsize_t iblock = 1 )
  {
    offset[ 0 ] = ioffset;
    stride[ 0 ] = istride;
    count[ 0 ] = icount;
    block[ 0 ] = iblock;
  };

  inline uint64_t
  view2dataset( const uint64_t& v_idx ) const
  {
    return offset[ 0 ] + ( v_idx / block[ 0 ] ) * ( stride[ 0 ] - 1 ) + v_idx;
  }

  static bool
  MinSynPtr( const NeuronLink& a, const NeuronLink& b )
  {
    return a.syn_ptr < b.syn_ptr;
  };
};

class H5SynapsesLoader
{
protected:
  hid_t file_id_, gid_; // hdf5 file pointer

  uint64_t& n_readSynapses;
  uint64_t& n_SynapsesInDatasets;

  uint64_t total_num_syns_;
  uint64_t global_offset_;


  hid_t syn_memtype_;

  uint64_t fixed_num_syns_;

  std::vector< NeuronLink >::const_iterator it_neuronLinks_;

  int NUM_PROCESSES;
  int RANK;

  std::vector< NeuronLink > neuronLinks;

  struct H5Dataset
  {
    hid_t dataset_id_;

    H5Dataset( const H5SynapsesLoader* loader, const char* datasetname )
    {
      dataset_id_ = H5Dopen2( loader->file_id_, datasetname, H5P_DEFAULT );
    }

    ~H5Dataset()
    {
      H5Dclose( dataset_id_ );
    }

    hid_t
    getId() const
    {
      return dataset_id_;
    }
  };

  H5Dataset* syn_dataset;

  size_t
  getNumberOfSynapses( H5Dataset& dataset )
  {
    hsize_t count;

    hid_t dataspace_id = H5Dget_space( dataset.getId() );
    H5Sget_simple_extent_dims(
      dataspace_id, &count, NULL ); // get vector length from x dataset length
    H5Sclose( dataspace_id );

    return count;
  }

public:
  H5SynapsesLoader( const std::string h5file,
    const std::vector< std::string > prop_names,
    uint64_t& n_readSynapses,
    uint64_t& n_SynapsesInDatasets,
    uint64_t fixed_num_syns,
    uint64_t lastSyn = 0 )
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
      syn_memtype_ = H5Tcreate( H5T_COMPOUND,
        sizeof( int )
          + sizeof( float ) * prop_names.size() ); // target + param values
      // target is always given
      H5Tinsert( syn_memtype_, "target", 0, H5T_NATIVE_INT ); // first entry in pool is target id
      // additionally load following columns
      for ( int i = 0; i < prop_names.size(); i++ )
        H5Tinsert( syn_memtype_, prop_names[ i ].c_str(), sizeof( int ) + i * sizeof( float ), H5T_NATIVE_FLOAT ); // following entries in pool are param values

      // get source neuron dataset
      loadNeuronLinks();
    }

    ~H5SynapsesLoader()
    {

      H5Tclose( syn_memtype_ );

      delete syn_dataset;

      H5Gclose( gid_ );
      H5Fclose( file_id_ );
    }

    void setLastSyn( uint64_t last )
    {
      if ( last < total_num_syns_ )
        total_num_syns_ = last;
    }

    /*
     * Returns the number of synapses (entries in syn dataset)
     */
    size_t getNumberOfSynapses()
    {
      // H5Dataset cell_dataset(this,"syn");
      return getNumberOfSynapses( *syn_dataset );
    }
    /*
     * returns of file pointer reached end of file
     */
    bool eof()
    {
      return total_num_syns_ <= global_offset_;
    }

    void removeNotNeededNeuronLinks()
    {
      std::vector< NeuronLink > tmp_neuronLinks;

      const uint64_t start = fixed_num_syns_ * RANK;
      const uint64_t end = fixed_num_syns_ * ( RANK + 1 );

      // sort out not necessary neuron links
      // best case: reduces neuronLinks size by around 1-1/NUM_PROCESSES
      for ( int i = 0; i < neuronLinks.size(); i++ )
      {
        // only synapses from global_offset_ to total_num_syns_ are loaded
        if ( /*neuronLinks[i].syn_ptr<total_num_syns_ &&*/ neuronLinks[ i ]
               .syn_ptr
            + neuronLinks[ i ].syn_n
          > global_offset_ )
        {
          const uint64_t entry_start =
            ( neuronLinks[ i ].syn_ptr - global_offset_ )
            % ( fixed_num_syns_ * NUM_PROCESSES );
          const uint64_t entry_end =
            ( neuronLinks[ i ].syn_ptr + neuronLinks[ i ].syn_n
              - global_offset_ )
            % ( fixed_num_syns_ * NUM_PROCESSES );

          if ( ( entry_start >= start && entry_start < end )
            || ( entry_end > start && entry_end <= end )
            || ( entry_start <= start && entry_end >= end ) )
            tmp_neuronLinks.push_back( neuronLinks[ i ] );
        }
      }
      neuronLinks.swap( tmp_neuronLinks );
    }


    /*
     * Load source neuron ids and store in vector
     */

    void loadNeuronLinks()
    {
      H5Dataset neuronLink_dataset( this, "neuron" );

      hid_t memtype = H5Tcreate( H5T_COMPOUND, sizeof( NeuronLink ) );
      H5Tinsert( memtype, "id", HOFFSET( NeuronLink, id ), H5T_NATIVE_INT );
      H5Tinsert(
        memtype, "syn_n", HOFFSET( NeuronLink, syn_n ), H5T_NATIVE_INT );
      H5Tinsert(
        memtype, "syn_ptr", HOFFSET( NeuronLink, syn_ptr ), H5T_NATIVE_ULLONG );

      hid_t dataspace_id = H5Dget_space( neuronLink_dataset.getId() );

      hsize_t count;
      H5Sget_simple_extent_dims( dataspace_id, &count, NULL );

      neuronLinks.resize( count );

      // load dataset only on one node
      if ( RANK == 0 )
      {
        hid_t memspace_id = H5Screate_simple( 1, &count, NULL );

        hid_t dxpl_id_ = H5Pcreate( H5P_DATASET_XFER );
        //H5Pset_dxpl_mpio( dxpl_id_, H5FD_MPIO_INDEPENDENT );

        H5Dread( neuronLink_dataset.getId(),
          memtype,
          memspace_id,
          dataspace_id,
          dxpl_id_,
          &neuronLinks[ 0 ] );
        H5Pclose( dxpl_id_ );
        H5Sclose( memspace_id );
      }
      // broadcast entries to all nodes
      MPI_Bcast( &neuronLinks[ 0 ],
        count * sizeof( NeuronLink ),
        MPI_CHAR,
        0,
        MPI_COMM_WORLD );

      H5Sclose( dataspace_id );

      // try to reduce memory consumption
      // could be moved to reading only a subset per rank
      removeNotNeededNeuronLinks();

      std::stable_sort(
        neuronLinks.begin(), neuronLinks.end(), H5View::MinSynPtr );

      // set iterator
      it_neuronLinks_ = neuronLinks.begin();
    }

    /**
     * search source neuron
     *
     * could be optimized using a binary search alg
     */
    void integrateSourceNeurons(
      NESTSynapseList & synapses, const H5View& view )
    {
      uint64_t index = view.view2dataset( 0 );

      for ( int i = 0; i < synapses.size(); i++ )
      {
        index = view.view2dataset( i );

        while ( it_neuronLinks_ < neuronLinks.end() )
        {
          if ( index >= ( it_neuronLinks_->syn_ptr + it_neuronLinks_->syn_n ) )
          {
            it_neuronLinks_++;
          }
          else if ( index < it_neuronLinks_->syn_ptr )
          {
            std::cout << "ERROR:"
                      << "index=" << index
                      << "\tsyn_ptr=" << it_neuronLinks_->syn_ptr << std::endl;
            break;
          }
          else
          {
            synapses[ i ].source_neuron_ = it_neuronLinks_->id;
            break;
          }
        }
      }
    }

    /*
     * Get num_syns synapses from datasets collectivly
     * Move file pointer to for next function call
     *
     */
    void iterateOverSynapsesFromFiles( NESTSynapseList & synapses )
    {
      /*std::vector<uint64_t> global_num_syns(NUM_PROCESSES);
      uint64_t private_num_syns = num_syns;

      MPI_Allgather(&private_num_syns, 1, MPI_UNSIGNED_LONG_LONG,
      &global_num_syns[0], 1, MPI_UNSIGNED_LONG_LONG, MPI_COMM_WORLD);
      uint64_t private_offset = std::accumulate(global_num_syns.begin(),
      global_num_syns.begin()+RANK, global_offset_);
      global_offset_ = std::accumulate(global_num_syns.begin()+RANK,
      global_num_syns.end(), private_offset); // for next iteration
      */

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

      synapses.resize( dataspace_view.count[ 0 ] );

      // setup collective read operation
      hid_t dxpl_id_ = H5Pcreate( H5P_DATASET_XFER );
      // H5Pset_dxpl_mpio(dxpl_id_, H5FD_MPIO_COLLECTIVE);
      //H5Pset_dxpl_mpio( dxpl_id_, H5FD_MPIO_INDEPENDENT );

      H5Dread( syn_dataset->getId(),
        syn_memtype_,
        memspace_id,
        dataspace_id,
        dxpl_id_,
        &synapses.property_pool_[0] );

      H5Pclose( dxpl_id_ );

      H5Sclose( memspace_id );
      H5Sclose( dataspace_id );

      // integrate NEST neuron id offset to synapses
      integrateSourceNeurons( synapses, dataspace_view );

      // observer variable
      n_readSynapses += dataspace_view.count[ 0 ];
    }
};

#endif
