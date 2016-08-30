/*
 * kernels.h
 *
 *  Created on: Jul 20, 2016
 *      Author: schumann
 */
#ifndef KERNELS_H_
#define KERNELS_H_

#include <iterator>
#include <vector>

#include <omp.h>

namespace h5import {

typedef std::vector<double> TokenArray;

/*
 * help class for thread buffers
 * each omp thread gets its own buffer
 */
template < typename T >
struct private_vector
{
    typedef T type_name;
    std::vector< std::vector< type_name > > all_vectors;

    private_vector() : all_vectors(omp_get_num_threads())
    {}

    std::vector< T >& operator()()
    {
        return all_vectors[omp_get_thread_num()];
    }
};

template < typename T >
struct manipulate_kernel
{
  typedef T type_name;

  virtual ~manipulate_kernel()
  {}

  virtual std::vector< type_name >&
  operator()( typename std::vector< type_name >::iterator begin, typename std::vector< type_name >::iterator end )
  {
    pv().resize(end-begin);
    std::copy(begin, end, pv().begin());
    return pv();
  }
private:
  private_vector< type_name > pv;
};

template < typename T >
struct kernel_combi
{
  typedef T type_name;
  std::vector< manipulate_kernel< type_name >* > kernels_;
  kernel_combi( )
  {
    manipulate_kernel< type_name >* k = new manipulate_kernel< type_name >( );
    kernels_.push_back( k );
  }

  ~kernel_combi()
  {
    for ( int i = 0; i < kernels_.size(); i++ )
      delete kernels_[ i ];
  }

  template < typename K >
  void
  push_back( const std::vector< type_name >& v )
  {
    K* k = new K( v );
    kernels_.push_back( static_cast< manipulate_kernel< type_name >* >( k ) );
  }

  std::vector<type_name>&
  operator()( typename std::vector<type_name>::iterator begin, typename std::vector<type_name>::iterator end )
  {
      pv().resize(end-begin);
      std::copy(begin, end, pv().begin());
      std::vector<type_name>* result_vector = &pv();
      for ( int i = 0; i < kernels_.size(); i++ ) {
          result_vector = &( *kernels_[ i ] )( result_vector->begin(), result_vector->end());
      }
      return *result_vector;
    }

  template < typename Tin >
  std::vector<type_name>&
  operator()( Tin* begin, Tin* end )
  {
      pv().resize(end-begin);
      std::copy(begin, end, pv().begin());
      std::vector<type_name>* result_vector = &pv();
      for ( int i = 0; i < kernels_.size(); i++ ) {
          result_vector = &( *kernels_[ i ] )( result_vector->begin(), result_vector->end());
      }
      return *result_vector;
  }

private:
  private_vector< type_name > pv;
};

template < typename T >
struct kernel_multi : public manipulate_kernel< T >
{
    typedef T type_name;

  std::vector< type_name > multis_;
  kernel_multi( TokenArray multis )
  {
    for ( int i = 0; i < multis.size(); i++ )
      multis_.push_back( multis[ i ] );
  }

  std::vector< type_name >
  operator()( typename std::vector<type_name>::iterator begin, typename std::vector<type_name>::iterator end )
  {
    const int n = end-begin;
    assert( n == multis_.size() );
    pv().resize(n);

    std::transform(begin, end, multis_.begin(), pv().begin(), std::multiplies<type_name>());
    return pv();
  }

private:
  private_vector< type_name > pv;
};

template < typename T >
struct kernel_add : public manipulate_kernel< T >
{
    typedef T type_name;

  std::vector< type_name > adds_;
  kernel_add( TokenArray adds )
  {
    for ( int i = 0; i < adds.size(); i++ )
      adds_.push_back( adds[ i ] );
  }
  std::vector< type_name >&
  operator()( typename std::vector<type_name>::iterator begin, typename std::vector<type_name>::iterator end )
  {
    const int n = end-begin;
    assert( n == adds_.size() );
    pv().resize(n);

    std::transform(begin, end, adds_.begin(), pv().begin(), std::plus<type_name>());
    return pv();
  }

private:
  private_vector< type_name > pv;
};

/**
 * Short Range Weight Adaptation based on delay
 */
template < typename T >
struct kernel_srwa : public manipulate_kernel< T >
{
  typedef T type_name;

  type_name lower;
  type_name upper;
  kernel_srwa( TokenArray& boundaries )
  {
    assert( boundaries.size() == 2 );
    lower = boundaries[ 0 ];
    upper = boundaries[ 1 ];
  }

  std::vector< type_name >&
  operator()( typename std::vector<type_name>::iterator begin, typename std::vector<type_name>::iterator end )
  {
    const int n = end-begin;
    assert( n == 5 );
    pv().resize(n);

    std::vector< type_name >& output = pv().begin();
    std::copy(begin, end, output.begin());

    const double Vprop = 0.8433734 * 1000.0;
    const double distance = output[0] * Vprop;
    if ( distance <= lower )
        output[1] *= 2.0;
    else if ( distance < upper )
        output[1] *= -2.0;

    return pv();
  }

private:
  private_vector< type_name > pv;
};

/*template <typename T>
struct kernel_rand : public manipulate_kernel<T> {
        std::vector<bool> rs_;
        librandom::RngPtr rng_;
        kernel_rand(const int& tid, const std::vector<bool>& rs): rs_(rs)
        {
                rng_ = kernel().rng_manager.get_rng( tid );
        }
        std::vector<T> operator()(std::vector<T> values)
        {
                assert(values.size() == 1);

                for (int i=0; i<rs_; i++)
                        if (rs_[i])
                                values[i] = rng_;

                return values;
        }
};*/

}; //end h5import namespace

#endif /* KERNELS_H_ */
