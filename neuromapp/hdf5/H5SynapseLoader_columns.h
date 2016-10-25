#include <hdf5.h>
#include <mpi.h>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>
#include <cassert>

#ifndef H5SYNAPSESLOADER_CLASS
#define H5SYNAPSESLOADER_CLASS

class H5SynapsesLoader
{
public:

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
    };

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

protected:
  hid_t file_id_, gid_; // hdf5 file pointer

  uint64_t& n_readSynapses;
  uint64_t& n_SynapsesInDatasets;
  uint64_t total_num_syns_;
  uint64_t global_offset_;
  uint64_t fixed_num_syns_;

  int NUM_PROCESSES;
  int RANK;
  

  std::vector< H5Dataset* > syn_datasets;

  size_t
  getNumberOfSynapses( H5Dataset& dataset );

public:
  H5SynapsesLoader( const std::string h5file,
    const std::vector< std::string > datasets,
    uint64_t& n_readSynapses,
    uint64_t& n_SynapsesInDatasets,
    uint64_t fixed_num_syns,
    uint64_t lastSyn = 0 );

    ~H5SynapsesLoader();

    void setLastSyn( uint64_t last );

    /*
     * Returns the number of synapses (entries in syn dataset)
     */
    size_t getNumberOfSynapses();
    /*
     * returns of file pointer reached end of file
     */
    bool eof();
    /*
     * Get num_syns synapses from datasets collectivly
     * Move file pointer to for next function call
     *
     */
    void iterateOverSynapsesFromFiles( std::vector<int> & buffer );
};

#endif
