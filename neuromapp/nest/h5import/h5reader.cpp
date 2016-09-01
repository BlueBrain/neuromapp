#include <hdf5.h>
#include <mpi.h>
#include <algorithm>
#include <numeric>
#include <vector>
#include <cassert>

#include "nest/h5import/h5reader.h"

using namespace h5import;

  h5reader::h5reader( const std::string& path,
                      const std::vector< std::string >& datasets,
                      const uint64_t& transfersize,
                      const uint64_t& limittotalsize)
        : file_id_      ( H5I_INVALID_HID ),
          gid_          ( H5I_INVALID_HID ),
          memtype_      ( H5I_INVALID_HID ),
          dataset_ptr_  ( NULL ),
          global_offset_( 0 ),
          totalsize_    ( limittotalsize ),
          transfersize_ ( transfersize ),
          num_compound_ ( datasets.size() )
  {
    MPI_Comm_size( MPI_COMM_WORLD, &num_processes_ );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank_ );


    hid_t fapl_id = H5Pcreate( H5P_FILE_ACCESS );
    file_id_ = H5Fopen( path.c_str(), H5F_ACC_RDONLY, fapl_id );
    H5Pclose( fapl_id );

    gid_ = H5Gopen( file_id_, "/", H5P_DEFAULT );
    dataset_ptr_ = new h5dataset( this, "syn" );

    // read size and adapt if set
    const unsigned long long  real_size = this->size();
    if ( real_size < totalsize_ )
         totalsize_ = real_size;

    //create memtype
    memtype_ = H5Tcreate( H5T_COMPOUND, sizeof( int ) + sizeof( float ) * num_compound_ );

    H5Tinsert( memtype_, "target", 0, H5T_NATIVE_INT );
    for ( int i = 0; i < datasets.size(); i++ )
        H5Tinsert( memtype_, datasets[ i ].c_str(), sizeof( int )+i*sizeof( float ), H5T_NATIVE_FLOAT );

    //currently no neuron link dataset available in test dataset
    //loadNeuronLinks();


  }

  h5reader::~h5reader()
    {
        H5Tclose( memtype_ );
        delete dataset_ptr_;
        H5Gclose( gid_ );
        H5Fclose( file_id_ );
    }

      /*
      * Load source neuron ids and store in vector
      */

     void h5reader::loadNeuronLinks()
     {
       h5dataset neuronLink_dataset( this, "neuron" );

       //layout of source neuron data set
       hid_t memtype = H5Tcreate( H5T_COMPOUND, sizeof( NeuronLink ) );
       H5Tinsert( memtype, "id", HOFFSET( NeuronLink, id ), H5T_NATIVE_INT );
       H5Tinsert( memtype, "syn_n", HOFFSET( NeuronLink, syn_n ), H5T_NATIVE_INT );
       H5Tinsert( memtype, "syn_ptr", HOFFSET( NeuronLink, syn_ptr ), H5T_NATIVE_ULLONG );

       //figure out entries
       const hsize_t count = neuronLink_dataset.size();
       neuronLinks_.resize( count );

       // load dataset only on one node
       if ( rank_ == 0 )
       {
         hid_t dataspace_id = H5Dget_space( neuronLink_dataset.id() );
         hid_t memspace_id = H5Screate_simple( 1, &count, NULL );

         hid_t dxpl_id_ = H5Pcreate( H5P_DATASET_XFER );

         H5Dread( neuronLink_dataset.id(),
           memtype,
           memspace_id,
           dataspace_id,
           dxpl_id_,
           &neuronLinks_[ 0 ] );

         H5Pclose( dxpl_id_ );
         H5Sclose( memspace_id );
         H5Sclose( dataspace_id );
       }
       // broadcast entries to all nodes
       MPI_Bcast( &neuronLinks_[ 0 ], count * sizeof( NeuronLink ), MPI_CHAR, 0, MPI_COMM_WORLD );

       // try to reduce memory consumption
       // could be moved to reading only a subset per rank
       removeNotNeededNeuronLinks();

       //sort to find entires afterwards faster
       std::stable_sort( neuronLinks_.begin(), neuronLinks_.end(), h5view::MinSynPtr );
     }

     void h5reader::removeNotNeededNeuronLinks()
    {
        std::vector< NeuronLink > tmp_neuronLinks;
        const uint64_t start = transfersize_ * rank_;
        const uint64_t end = transfersize_ * ( rank_ + 1 );

        // sort out not necessary neuron links
        // best case: reduces neuronLinks size by around 1-1/NUM_PROCESSES
        for ( int i = 0; i < neuronLinks_.size(); i++ )
        {
         // only synapses from global_offset_ to sizelimit are loaded
         if ( neuronLinks_[ i ].syn_ptr + neuronLinks_[ i ].syn_n > global_offset_ )
         {
           const uint64_t entry_start =
             ( neuronLinks_[ i ].syn_ptr - global_offset_ )
             % ( transfersize_ * num_processes_ );
           const uint64_t entry_end =
             ( neuronLinks_[ i ].syn_ptr + neuronLinks_[ i ].syn_n
               - global_offset_ )
             % ( transfersize_ * num_processes_ );

           if ( ( entry_start >= start && entry_start < end )
             || ( entry_end > start && entry_end <= end )
             || ( entry_start <= start && entry_end >= end ) )
             tmp_neuronLinks.push_back( neuronLinks_[ i ] );
         }
        }
        neuronLinks_.swap( tmp_neuronLinks );
    }


    void h5reader::integrateSourceNeurons(NESTSynapseList& synapses, const h5view& view )
    {
    //use private iterator to be thread safe
    std::vector< NeuronLink >::const_iterator it_neuronLinks = neuronLinks_.begin();
    //start at first entry
    uint64_t index = view.view2dataset( 0 );
        for ( int i = 0; i < synapses.size(); i++ ) {
            index = view.view2dataset( i );
            while ( it_neuronLinks < neuronLinks_.end() ) {
                if ( index >= ( it_neuronLinks->syn_ptr + it_neuronLinks->syn_n ) )
                    it_neuronLinks++;
                else if ( index < it_neuronLinks->syn_ptr ) {
                  std::cout << "ERROR:"
                            << "index=" << index
                            << "\tsyn_ptr=" << it_neuronLinks->syn_ptr << std::endl;
                  break;
                }
                else {
                  synapses[ i ].source_neuron_ = it_neuronLinks->id;
                  break;
                }
            }
        }
    }

    /*
     * Get num_syns synapses from datasets collectively
     * Move file pointer to for next function call
     *
     */
    void h5reader::readblock( NESTSynapseList& synapses, h5view& dataspace_view )
    {
      hsize_t private_offset = transfersize_ * rank_ + global_offset_;

      hssize_t count = std::min( ( hssize_t ) transfersize_,
        ( ( hssize_t ) totalsize_ - ( hssize_t ) private_offset ) );
      if ( count < 0 )
        count = 0;

      dataspace_view.count[0] = count;
      dataspace_view.offset[0] = private_offset;

      hid_t dataspace_id = H5Dget_space( dataset_ptr_->id() );
      hid_t memspace_id = H5I_INVALID_HID;

      // be careful if there are no entries to load
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
      hid_t dxpl_id = H5Pcreate( H5P_DATASET_XFER );

      H5Dread( dataset_ptr_->id(),
               memtype_,
               memspace_id,
               dataspace_id,
               dxpl_id,
               &synapses.property_pool_[0] );

      H5Pclose( dxpl_id );

      H5Sclose( memspace_id );
      H5Sclose( dataspace_id );

      //increase offset for next iteration
      global_offset_ += transfersize_ * num_processes_;
    }

