#include <hdf5.h>
#include <mpi.h>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>

#include "nest/h5import/NESTNodeSynapse.h"

#ifndef H5IMPORT_H5READER_CLASS
#define H5IMPORT_H5READER_CLASS

namespace h5import {

class h5reader
{
public:
    struct h5view
    {
      hsize_t offset[ 1 ];
      hsize_t stride[ 1 ];
      hsize_t count[ 1 ];
      hsize_t block[ 1 ];

      h5view( hsize_t icount,
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
  hid_t file_id_, gid_; // hdf5 file pointer
  hid_t memtype_;
  h5dataset* dataset_ptr_;

  unsigned long long global_offset_;
  unsigned long long totalsize_;
  unsigned long long transfersize_;
  unsigned int num_compound_;

  int num_processes_;
  int rank_;

  size_t size( h5dataset* dataset );

public:
    h5reader( const std::string& h5file,
              const std::vector< std::string >& datasets,
              const unsigned long long& transfersize,
              const unsigned long long& limittotalsize = -1 );

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
    void readblock( NESTSynapseList* synapses );
};


}; //end of h5import namespace

#endif
