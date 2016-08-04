/*
 * kernels.h
 *
 *  Created on: Jul 20, 2016
 *      Author: schumann
 */

#ifndef KERNELS_H_
#define KERNELS_H_

typedef std::vector<double> TokenArray;

template < typename T >
struct manipulate_kernel
{
  virtual std::vector< T >
  operator()( std::vector< T > values )
  {
    return values;
  }
};

template < typename T >
struct kernel_combi : public manipulate_kernel< T >
{
  std::vector< manipulate_kernel< T >* > kernels_;
  kernel_combi()
  {
    manipulate_kernel< T >* k = new manipulate_kernel< T >();
    kernels_.push_back( k );
  }

  ~kernel_combi()
  {
    for ( int i = 0; i < kernels_.size(); i++ )
      delete kernels_[ i ];
  }

  template < typename K >
  void
  push_back( const std::vector< T >& v )
  {
    K* k = new K( v );
    kernels_.push_back( static_cast< manipulate_kernel< T >* >( k ) );
  }
  std::vector< T >
  operator()( std::vector< T > values )
  {
    for ( int i = 0; i < kernels_.size(); i++ )
      values = ( *kernels_[ i ] )( values );
    return values;
  }
};

template < typename T >
struct kernel_multi : public manipulate_kernel< T >
{
  std::vector< T > multis_;
  kernel_multi( TokenArray multis )
  {
    for ( int i = 0; i < multis.size(); i++ )
      multis_.push_back( multis[ i ] );
  }
  std::vector< T >
  operator()( std::vector< T > values )
  {
    assert( values.size() == multis_.size() );

    for ( int i = 0; i < values.size(); i++ )
      values[ i ] *= multis_[ i ];
    return values;
  }
};

template < typename T >
struct kernel_add : public manipulate_kernel< T >
{
  std::vector< T > adds_;
  kernel_add( TokenArray adds )
  {
    for ( int i = 0; i < adds.size(); i++ )
      adds_.push_back( adds[ i ] );
  }
  std::vector< T >
  operator()( std::vector< T > values )
  {
    assert( values.size() == adds_.size() );

    for ( int i = 0; i < values.size(); i++ )
      values[ i ] += adds_[ i ];
    return values;
  }
};

template < typename T >
struct kernel_csaba : public manipulate_kernel< T >
{
  T lower;
  T upper;
  kernel_csaba( TokenArray& boundaries )
  {
    assert( boundaries.size() == 2 );
    lower = boundaries[ 0 ];
    upper = boundaries[ 1 ];
  }
  std::vector< T >
  operator()( std::vector< T > values )
  {
    assert( values.size() == 5 );

    const double Vprop = 0.8433734 * 1000.0;
    const double distance = values[ 0 ] * Vprop;
    if ( distance <= lower )
      values[ 1 ] *= 2.0;
    else if ( distance < upper )
      values[ 1 ] *= -2.0;

    return values;
  }
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
