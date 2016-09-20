#include <iostream>
#include <vector>


#ifndef NESTNODESYNAPSE_CLASS
#define NESTNODESYNAPSE_CLASS

namespace h5import
{
/**
 * GIDCollection used for mapping gids (e.g. NEST uses an offset of one)
 */
class GIDCollection
{
private:
    size_t offset;
    size_t size;
public:
    GIDCollection(const size_t& offset = 0, const size_t& size = -1):
        offset(offset),
        size(size)
    {}

    inline size_t operator[](const size_t& idx) const
    {
        return idx+offset;
    }
};

/**
 * buffer class for mpi communication
 */
template <typename T>
class mpi_buffer
{
private:
    std::vector<T> buf;
    size_t n;
    size_t readalready;
public:
    mpi_buffer(size_t n, bool fill=false): n(n), readalready(0)
    {
        if (fill)
            buf.resize(n);
        else
            buf.reserve(n);
    }
    T* begin()
    {
        return &buf[readalready];
    }
    size_t size()
    {
        return buf.size()-readalready;
    }
    void push_back(const T& v)
    {
        buf.push_back(v);
    }
    T& operator[](const size_t& idx)
    {
    return buf[idx+readalready];
    }
    const T& operator[](const size_t& idx) const
    {
    return buf[idx+readalready];
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

/**
 * reference object to an entry of SynapseList
 * 
 */
struct SynapseRef
{
    uint32_t& source_neuron_;
    uint32_t& target_neuron_;
    uint32_t& node_id_;

  class ParamPtr
  {
  private:
    float* ptr_; //union datatype is probably nicer
  public:
    size_t n_;
    ParamPtr( float* ptr, const size_t& n )
      : ptr_( ptr ), n_(n)
    {
    }
    float& operator[]( const unsigned int& i )
    {
      return *( ptr_ + i );
    }
    float* begin() const
    {
      return ptr_;
    }
    float* end() const
    {
      return ptr_ + n_;
    }
    size_t size() const
    {
      return n_;
    }
  } params_;

  /**
   * Create SynapseRef object using the passed variable as memory locations
   */
  SynapseRef( uint32_t& source_neuron,
              uint32_t& node_id,
              const uint32_t& num_params,
    		  char* pool_entry )
    : source_neuron_( source_neuron )
    , target_neuron_( *( reinterpret_cast< uint32_t* >( pool_entry ) ) )
    , node_id_( node_id )
    , params_( reinterpret_cast< float* >( pool_entry ) + 1, num_params )
  {}

  /**
   * Serialize and write bytes to the passed mpi_buffer
   * i is used as starting address in the mpi_buffer
   */
  size_t serialize( mpi_buffer<int>& buf, size_t i )
  {
    const size_t begin_i = i;
    buf[i++] = source_neuron_;
    buf[i++] = target_neuron_;
    buf[i++] = node_id_;

    for ( int j = 0; j < params_.size(); j++ )
      buf[i++] = *reinterpret_cast< int* >( &params_[ j ] ) ;

    return i-begin_i;
  }
  /**
   * deserialize mpi buffer and copy back into the memory
   * locations given by the SynapseRef object
   * i is used as the starting address in the mpi_buffer
   */
  void deserialize( mpi_buffer<int>& buf, size_t i )
  {
    source_neuron_ = buf[i++];
    target_neuron_ = buf[i++];
    node_id_ = buf[i++];

    for ( int j = 0; j < params_.size(); j++ )
      params_[ j ] = *reinterpret_cast< float* >( &buf[i++] );
  }

  SynapseRef& operator=( const SynapseRef& r )
  {
    if (this == &r)
        return *this;
    source_neuron_ = r.source_neuron_;
    target_neuron_ = r.target_neuron_;
    node_id_ = r.node_id_;

    params_.n_ = r.params_.n_;
    std::copy( r.params_.begin(), r.params_.end(), params_.begin() );
    return *this;
  }
};


class SynapseList
{
private:
  std::vector< uint32_t > source_neurons;
  std::vector< uint32_t > node_id_;
  size_t num_params_;
public:
	std::vector< char > property_pool_;
 /**
  * Create a SynapseList with a the given number of parameters
  */
  SynapseList( const size_t& num_params ): num_params_( num_params )
  {}

  inline size_t get_num_params() const
  {
	  return num_params_;
  }
	  
  /**
   * Returns the SynapseRef object for entry idx
   */
  SynapseRef operator[]( size_t idx )
  {
    const size_t pool_idx = idx * sizeof_pool_entry();
    return SynapseRef( source_neurons[ idx ],
      node_id_[ idx ],
      num_params_,
      &property_pool_[ pool_idx ] );
  }
  void resize( const size_t& n )
  {
    source_neurons.resize( n );
    node_id_.resize( n );
    property_pool_.resize( n * sizeof_pool_entry() );
  }
  inline void clear()
  {
    source_neurons.clear();
    node_id_.clear();
    property_pool_.clear();
  }
  inline size_t size() const
  {
    return source_neurons.size();
  }

  inline size_t sizeof_pool_entry()
  {
      return num_params_ * sizeof( float ) + sizeof( uint32_t );
  }

  inline size_t sizeof_entry()
  {
      return 2*sizeof( uint32_t ) + sizeof_pool_entry();
  }
};

};

#endif
