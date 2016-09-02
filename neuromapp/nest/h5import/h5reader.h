#include <hdf5.h>
#include <mpi.h>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>

#include "nest/h5import/SynapseList.h"

#ifndef H5IMPORT_H5READER_CLASS
#define H5IMPORT_H5READER_CLASS

namespace h5import {

class h5reader
{
public:
    /*
     *  store data from neuron link dataset
     */
    struct NeuronLink
    {
      int id;
      int syn_n;
      uint64_t syn_ptr;
    };

    struct h5view
    {
      hsize_t offset[ 1 ];
      hsize_t stride[ 1 ];
      hsize_t count[ 1 ];
      hsize_t block[ 1 ];

      h5view( hsize_t icount = 0,
        hsize_t ioffset = 0,
        hsize_t istride = 1,
        hsize_t iblock = 1 )
      {
        offset[ 0 ] = ioffset;
        stride[ 0 ] = istride;
        count[ 0 ] = icount;
        block[ 0 ] = iblock;
      };

      inline hsize_t
      view2dataset( const hsize_t& v_idx ) const
      {
        return offset[ 0 ] + ( v_idx / block[ 0 ] ) * ( stride[ 0 ] - 1 ) + v_idx;
      }
      static bool
      MinSynPtr( const NeuronLink& a, const NeuronLink& b )
      {
        return a.syn_ptr < b.syn_ptr;
      };
    };

    class h5dataset
    {
    private:
        hid_t id_;

    public:
        h5dataset( const h5reader* loader, const std::string& datasetname )
        {
            id_ = H5Dopen2( loader->file_id_, datasetname.c_str(), H5P_DEFAULT );
        }

        ~h5dataset()
        {
          H5Dclose( id_ );
        }

        hid_t& id()
        {
          return id_;
        }

        hsize_t size() const
        {
            hid_t dataspace_id = H5Dget_space( id_ );
            hsize_t count;
            H5Sget_simple_extent_dims(dataspace_id, &count, NULL );
            H5Sclose( dataspace_id );
            return count;
        }
    };

protected:
  // hdf5 file pointer
  hid_t file_id_, gid_;
  hid_t memtype_;
  h5dataset* dataset_ptr_;

  //pointer in syn dataset
  uint64_t global_offset_;
  //considered pointer to last entry
  uint64_t totalsize_;
  //number of data transfered on each read call
  uint64_t transfersize_;
  //number of columns in syn dataset
  uint32_t num_compound_;

  int num_processes_;
  int rank_;

  std::vector< NeuronLink > neuronLinks_;

  /*
   *  return size from dataset
   */
  size_t size( h5dataset* dataset ) const;

  /*
   *  load neuron links from hdf5 file
   */
  void loadNeuronLinks();

  /*
   *  remove not needed neuron links to reduce memory footprint
   *  function is memory intense
   */
  void removeNotNeededNeuronLinks();

public:
    h5reader( const std::string& h5file,
              const std::vector< std::string >& datasets,
              const uint64_t& transfersize,
              const uint64_t& limittotalsize = -1 );

    ~h5reader();

    /*
     * Returns the number of entires in dataset
     */
    inline size_t size()
    {
        return dataset_ptr_->size();
    }

    /*
     * returns of file pointer reached end of file
     */
    inline bool eof() const
    {
        return totalsize_ <= global_offset_;
    }

    /*
     * read block from dataset and move internal pointer forward
     */
    void readblock( SynapseList& synapses, h5view& dataspace_view );

    /*
    * search source neuron in neuronlinks and integrate them in the synapse list
    */
    void integrateSourceNeurons( SynapseList& synapses, const h5view& view );
};
}; //end of h5import namespace

#endif
