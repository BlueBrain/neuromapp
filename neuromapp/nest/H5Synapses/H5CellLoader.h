#include "hdf5.h"
#include <vector>

class H5CellLoader
{
protected:
  hid_t file_id_, gid_; // hdf5 file pointer

  struct H5Dataset
  {
    hid_t dataset_id_;

    H5Dataset( const H5CellLoader* loader, const std::string& datasetname )
    {
      dataset_id_ =
        H5Dopen2( loader->file_id_, datasetname.c_str(), H5P_DEFAULT );
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

  size_t
  getNumberOfCells( H5Dataset& cell_dataset )
  {

    hsize_t count;

    hid_t dataspace_id = H5Dget_space( cell_dataset.getId() );
    H5Sget_simple_extent_dims(
      dataspace_id, &count, NULL ); // get vector length from x dataset length
    H5Sclose( dataspace_id );

    return count;
  }

public:
  H5CellLoader( const std::string& h5file )
  {
    file_id_ = H5Fopen( h5file.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT );
    gid_ = H5Gopen( file_id_, "/", H5P_DEFAULT );
  }
  ~H5CellLoader()
  {
    H5Gclose( gid_ );
    H5Fclose( file_id_ );
  }

  size_t
  getNumberOfCells( const std::string& datasetname )
  {
    H5Dataset cell_dataset( this, datasetname );
    return getNumberOfCells( cell_dataset );
  }


  /*
   * Load subnet dataset from hdf5 file
   */
  void
  loadSubnets( const uint64_t& numberOfNeurons,
    const std::string& subnet_name,
    NESTNeuronList& neurons )
  {
    // need subnet information of all nodes
    std::vector< int > buffer_int( numberOfNeurons );

    H5Dataset dset( this, subnet_name );

    hsize_t count = numberOfNeurons;

    hid_t memspace_id = H5Screate_simple( 1, &count, NULL );
    H5Dread( dset.getId(),
      H5T_NATIVE_INT,
      memspace_id,
      H5S_ALL,
      H5P_DEFAULT,
      &buffer_int[ 0 ] );
    H5Sclose( memspace_id );

    for ( int j = 0; j < numberOfNeurons; j++ )
      neurons.setSubnet( j, buffer_int[ j ] );
  }


  /*
   * Load parameters from hdf5 file for all local nodes
   * numberOfNeurons: number of loaded neurons
   * mod_offset: id of first new created neuron
   * neurons: output list
   */
  void
  loadLocalParameters( const std::vector< std::string >& dataset_names,
    const uint64_t numberOfNeurons,
    const int mod_offset,
    NESTNeuronList& neurons )
  {

    // target neuron ids are based on neuron_id MOD numberOfNodes:
    // So the target neuron ids are: Node_id, Node_id+numberOfNodes,
    // Node_id+2*numberOfNodes, ..

    hsize_t count = numberOfNeurons;


    int num_processes = nest::kernel().mpi_manager.get_num_processes();
    int rank = nest::kernel().mpi_manager.get_rank();


    // NEST neuron id mapping
    int64_t first_neuron =
      ( rank % num_processes ) - ( mod_offset % num_processes );
    if ( first_neuron < 0 )
      first_neuron += num_processes;

    hsize_t offset = first_neuron;


    // calculate number of neurons per node based on modulos distribution
    hsize_t local_count = numberOfNeurons / num_processes
      + ( ( numberOfNeurons % num_processes ) > 0 );
    if ( mod_offset % num_processes > rank
      || ( mod_offset + numberOfNeurons - 1 ) % num_processes < rank )
      local_count--;

    hsize_t stride = nest::kernel().mpi_manager.get_num_processes();
    hsize_t block = 1;

    hid_t filespace_id = H5Screate_simple( 1, &count, NULL );
    H5Sselect_hyperslab(
      filespace_id, H5S_SELECT_SET, &offset, &stride, &local_count, &block );

    hid_t memspace_id = H5Screate_simple( 1, &local_count, NULL );

    // only parameters for local nodes are needed
    std::vector< float > buffer_flt( local_count );
    for ( int i = 0; i < dataset_names.size(); i++ )
    {
      H5Dataset dset( this, dataset_names[ i ] );
      H5Dread( dset.getId(),
        H5T_NATIVE_FLOAT,
        memspace_id,
        filespace_id,
        H5P_DEFAULT,
        &buffer_flt[ 0 ] );

      // hyperslab cannot be used, because neurons object can contain different
      // datatypes than floats!!
      int j_local = 0;
      for ( int j = first_neuron; j < numberOfNeurons; j += num_processes )
      {
        neurons.neuron_parameters_[ j * 13 + i ] = buffer_flt[ j_local ];
        j_local++;
      }
    }

    H5Sclose( memspace_id );
  }
};
