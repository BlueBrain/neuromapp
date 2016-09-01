#include <iostream>
#include <vector>

#include "nest/h5import/fakenestkernel/nest_kernel.h"

#ifndef NESTNODESYNAPSE_CLASS
#define NESTNODESYNAPSE_CLASS

namespace h5import
{

struct GIDCollection
{
    size_t offset;
    size_t size;

    GIDCollection(const size_t& offset = 0, const size_t& size = -1):
        offset(offset),
        size(size)
    {}

    inline size_t operator[](const size_t& idx) const
    {
        assert(idx<size);
        return idx+offset;
    }
};

typedef GIDCollection GIDCollectionDatum;

template <typename T>
struct mpi_buffer
{
    std::vector<T> buf;
    size_t n;
    size_t readalready;
    mpi_buffer(size_t n, bool fill=false): n(n), readalready(0)
    {
        if (fill)
            buf.resize(n);
        else
            buf.reserve(n);
    }
    T* begin()
    {
        return &buf[0];
    }
    size_t size()
    {
        return buf.size();
    }
    void push_back(const T& v)
    {
        //assert(buf.size()>n);
        buf.push_back(v);
    }
    T& operator[](const size_t& idx)
    {
    return buf[idx];
    }
    const T& operator[](const size_t& idx) const
    {
    return buf[idx];
    }
    T& pop_front()
    {
        readalready++;
        return *(buf.begin()+readalready-1);
    }
    void clear()
    {
        buf.clear();
        readalready = 0;
    }
};

struct NESTSynapseRef
{
    uint32_t& source_neuron_;
    uint32_t& target_neuron_;
    uint32_t& node_id_;

  struct ParamPtr
  {
    float* ptr_;
    size_t n_;
    ParamPtr( float* ptr, const size_t& n )
      : ptr_( ptr ), n_(n)
    {
    }
    float& operator[]( const unsigned int& i )
    {
      return *( ptr_ + i );
    }
    float*
    begin() const
    {
      return ptr_;
    }
    float*
    end() const
    {
      return ptr_ + n_;
    }
    size_t
    size()
    {
      return n_;
    }
  } params_;

  NESTSynapseRef( uint32_t& source_neurons,
          uint32_t& node_id,
    const size_t& num_params,
    char* pool_entry )
    : source_neuron_( source_neurons )
    , target_neuron_( *( reinterpret_cast< uint32_t* >( pool_entry ) ) )
    , node_id_( node_id )
    , params_( reinterpret_cast< float* >( pool_entry ) + 1, num_params )
  {}

  int
  serialize( mpi_buffer<int>& buf, size_t i )
  {
    //const int begin_size = buf.size();
    const size_t begin_i = i;
    buf[i++] = source_neuron_;
    buf[i++] = target_neuron_;
    buf[i++] = node_id_;

    for ( int j = 0; j < params_.size(); j++ )
      buf[i++] = *reinterpret_cast< int* >( &params_[ j ] ) ;

    return i-begin_i;
  }
  void
  deserialize( mpi_buffer<int>& buf, size_t i )
  {
    source_neuron_ = buf[i++];
    target_neuron_ = buf[i++];
    node_id_ = buf[i++];

    for ( int j = 0; j < params_.size(); j++ )
      params_[ j ] = *reinterpret_cast< float* >( &buf[i++] );
  }
  void
  integrateMapping( const GIDCollection& gidc )
  {
    source_neuron_ = gidc[ source_neuron_ ];
    target_neuron_ = gidc[ target_neuron_ ];

    node_id_ = kernel().mpi_manager.suggest_rank( target_neuron_ );
  }

  NESTSynapseRef&
  operator=( const NESTSynapseRef& r )
  {
    source_neuron_ = r.source_neuron_;
    target_neuron_ = r.target_neuron_;
    node_id_ = r.node_id_;

    params_.n_ = r.params_.n_;
    std::copy( r.params_.begin(), r.params_.end(), params_.begin() );
    return *this;
  }

  NESTSynapseRef&
  swap( NESTSynapseRef& r )
  {
    // create buf object
      uint32_t source_neuron_tmp;
      uint32_t node_id_tmp;
    std::vector< char > pool_tmp(
      params_.size() * sizeof( float ) + sizeof( int ) );
    NESTSynapseRef buf(
      source_neuron_tmp, node_id_tmp, params_.size(), &pool_tmp[0] );

    buf = *this;
    *this = r;
    r = buf;

    return *this;
  }
};


struct NESTSynapseList
{
  std::vector< uint32_t > source_neurons;
  std::vector< uint32_t > node_id_;
  std::vector< char > property_pool_;
  std::vector< std::string > prop_names_;
  int num_params_;

  NESTSynapseList(): num_params_(0)
  {}

  void set_properties( const std::vector< std::string >& prop_names )
  {
      prop_names_ = prop_names;
      num_params_ = prop_names.size();
  }
  NESTSynapseRef operator[]( std::size_t idx )
  {
    const int pool_idx = idx * sizeof_pool_entry();
    return NESTSynapseRef( source_neurons[ idx ],
      node_id_[ idx ],
      num_params_,
      &property_pool_[ pool_idx ] );
  };
  void
  resize( const int& n )
  {
    source_neurons.resize( n );
    node_id_.resize( n );
    property_pool_.resize( n * sizeof_pool_entry() );
  }
  void
  clear()
  {
    source_neurons.clear();
    node_id_.clear();
    property_pool_.clear();
  }
  size_t
  size() const
  {
    return source_neurons.size();
  }

  inline size_t sizeof_pool_entry()
  {
      return num_params_ * sizeof( float ) + sizeof(int);
  }

  size_t sizeof_entry()
  {
      return 2* sizeof(int) + sizeof_pool_entry();
  }
};

};

#endif
