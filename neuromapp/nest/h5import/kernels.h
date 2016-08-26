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


typedef std::vector<double> TokenArray;

template < typename T >
struct manipulate_kernel
{
  typedef T type_name;
  manipulate_kernel( const int& num_threads ):
      values_per_thread( num_threads )
  {}

  virtual ~manipulate_kernel()
  {}

  virtual std::vector< type_name >&
  operator()( const int& thrd, typename std::vector< type_name >::iterator begin, typename std::vector< type_name >::iterator end )
  {
    values_per_thread[thrd].resize(end-begin);
    std::copy(begin, end, values_per_thread[thrd].begin());
    return values_per_thread[thrd];
  }
private:
  std::vector< std::vector< type_name > > values_per_thread;
};

template < typename T >
struct kernel_combi
{
  typedef T type_name;
  std::vector< manipulate_kernel< type_name >* > kernels_;
  kernel_combi( const int& num_threads ): values_per_thread(num_threads)
  {
    manipulate_kernel< type_name >* k = new manipulate_kernel< type_name >( num_threads );
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
  operator()( const int& thrd, typename std::vector<type_name>::iterator begin, typename std::vector<type_name>::iterator end )
  {
      values_per_thread[thrd].resize(end-begin);
      std::copy(begin, end, values_per_thread[thrd].begin());
      std::vector<type_name>* result_vector = &values_per_thread[thrd];
      for ( int i = 0; i < kernels_.size(); i++ ) {
          result_vector = &( *kernels_[ i ] )( thrd, result_vector->begin(), result_vector->end());
      }
      return *result_vector;
    }

  template < typename Tin >
  std::vector<type_name>&
  operator()( const int& thrd, Tin* begin, Tin* end )
  {
      values_per_thread[thrd].resize(end-begin);
      std::copy(begin, end, values_per_thread[thrd].begin());
      std::vector<type_name>* result_vector = &values_per_thread[thrd];
      for ( int i = 0; i < kernels_.size(); i++ ) {
          result_vector = &( *kernels_[ i ] )( thrd, result_vector->begin(), result_vector->end());
      }
      return *result_vector;
  }

private:
  std::vector< std::vector< type_name > > values_per_thread;
};

template < typename T >
struct kernel_multi : public manipulate_kernel< T >
{
    typedef T type_name;

  std::vector< type_name > multis_;
  kernel_multi( const int& num_threads, TokenArray multis ): manipulate_kernel< type_name >(num_threads), values_per_thread( num_threads )
  {
    for ( int i = 0; i < multis.size(); i++ )
      multis_.push_back( multis[ i ] );
  }

  std::vector< type_name >
  operator()( const int& thrd, typename std::vector<type_name>::iterator begin, typename std::vector<type_name>::iterator end )
  {
    const int n = end-begin;
    assert( n == multis_.size() );
    values_per_thread[thrd].resize(n);

    std::transform(begin, end, multis_.begin(), values_per_thread[thrd].begin(), std::multiplies<type_name>());
    return values_per_thread[thrd];
  }

private:
  std::vector< std::vector< type_name > > values_per_thread;
};

template < typename T >
struct kernel_add : public manipulate_kernel< T >
{
    typedef T type_name;

  std::vector< type_name > adds_;
  kernel_add( const int& num_threads, TokenArray adds ): manipulate_kernel< type_name >(num_threads), values_per_thread( num_threads )
  {
    for ( int i = 0; i < adds.size(); i++ )
      adds_.push_back( adds[ i ] );
  }
  std::vector< type_name >&
  operator()( const int& thrd, typename std::vector<type_name>::iterator begin, typename std::vector<type_name>::iterator end )
  {
    const int n = end-begin;
    assert( n == adds_.size() );
    values_per_thread[thrd].resize(n);

    std::transform(begin, end, adds_.begin(), values_per_thread[thrd].begin(), std::plus<type_name>());
    return values_per_thread[thrd];
  }

private:
  std::vector< std::vector< type_name > > values_per_thread;
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
  kernel_srwa( const int& num_threads, TokenArray& boundaries ): manipulate_kernel< type_name >(num_threads), values_per_thread( num_threads )
  {
    assert( boundaries.size() == 2 );
    lower = boundaries[ 0 ];
    upper = boundaries[ 1 ];
  }

  std::vector< type_name >&
  operator()( const int& thrd, typename std::vector<type_name>::iterator begin, typename std::vector<type_name>::iterator end )
  {
    const int n = end-begin;
    assert( n == 5 );
    values_per_thread[thrd].resize(n);

    std::vector< type_name >& output = values_per_thread[thrd].begin();
    std::copy(begin, end, output.begin());

    const double Vprop = 0.8433734 * 1000.0;
    const double distance = output[0] * Vprop;
    if ( distance <= lower )
        output[1] *= 2.0;
    else if ( distance < upper )
        output[1] *= -2.0;

    return values_per_thread[thrd];
  }

private:
  std::vector< std::vector< type_name > > values_per_thread;
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

#endif /* KERNELS_H_ */
