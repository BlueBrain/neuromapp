/*
 * Neuromapp - connector_base.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CONNECTOR_BASE_H
#define CONNECTOR_BASE_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <cassert>

#include "nest/synapse/node.h"
#include "nest/synapse/event.h"
#include "nest/synapse/scheduler.h"

// when to truncate the recursive instantiation
#define K_CUTOFF 8

/* Removed suicide_and_resurrect function */

namespace nest
{

// base class to provide interface to decide
// - homogeneous connector (containing =1 synapse type)
//    -- which synapse type stored (syn_id)
// - heterogeneous connector (containing >1 synapse type)
class ConnectorBase
{

/* Removed many functions that didnt relate to Connector build or send stage.
 * Some functions include getters and setters for synapse status, connections,
 * target gid's and homogeneous_model check.
 * Assume all models are homogeneous.
 */
public:
  ConnectorBase();

  virtual void send( event& e, thread t ) = 0;

  // destructor needed to delete connections
  virtual ~ConnectorBase(){};

  inline double
  get_t_lastspike() const
  {
    return t_lastspike_;
  }

  inline void
  set_t_lastspike( const double t_lastspike )
  {
    t_lastspike_ = t_lastspike;
  }

  virtual size_t get_size () const = 0;

private:
  double t_lastspike_;
};

template <typename ConnectionT>
class vector_like : public ConnectorBase

/* Removed functions: at() and send_secondary() */
{
public:
  virtual ConnectorBase& push_back (const ConnectionT& c) = 0;
  virtual size_t get_size () const = 0;
};

// homogeneous connector containing K entries
template < size_t K, typename ConnectionT >
class Connector : public vector_like<ConnectionT>
{
  ConnectionT C_[ K ];

public:
  /**
   * Creates a new connector of sizes K by adding a new connection to a connector of size K - 1
   */
  Connector( const Connector<K-1, ConnectionT>& Cm1, const ConnectionT& c )
  {
    for ( size_t i = 0; i < K - 1; i++ )
        C_[ i ] = Cm1.get_C()[ i ];
    C_[ K - 1 ] = c;
  }

  void
  send( event& e, thread t )
  {
    //synindex syn_id = C_[ 0 ].get_syn_id();
    for ( size_t i = 0; i < K; i++ )
    {
      //e.set_port( i );
      C_[ i ].send( e,
        ConnectorBase::get_t_lastspike());
    }
    ConnectorBase::set_t_lastspike( e.get_stamp().get_ms() );
  }

  /**
   * Add a connection to the connector
   * @param c the connection to add.
   * @return A connector of size K+1
   */
  ConnectorBase& push_back( const ConnectionT& c )
  {
    /* Simplified push_back function by removing the call to suicide_and_ressurect,
     * which used a special NEST-specific allocator */

    ConnectorBase* p = new Connector<K + 1,ConnectionT>(*this, c);
    delete this;
    return *p;
  }

  /**
   * Getter for the size of C_
   */
  size_t get_size() const{ return K; }

  /**
   * Getter for the connection container, C_
   */
  const ConnectionT*
  get_C() const
  {
    return C_;
  }
};

//check ...
// homogeneous connector containing 1 entry (specialization to define constructor)
template < typename ConnectionT >
class Connector< 1, ConnectionT > : public vector_like<ConnectionT>
{
  ConnectionT C_[ 1 ];

public:
  Connector( const ConnectionT& c )
  {
    C_[ 0 ] = c;
  }

  /** Apparently these functions must be copy pasted into each template
   *  specialization so that they are not virtual classes... BAD
   */
  void
  send( event& e, thread t )
  {
    C_[ 0 ].send( e, ConnectorBase::get_t_lastspike());
    ConnectorBase::set_t_lastspike( e.get_stamp().get_ms() );
  }

  ConnectorBase& push_back( const ConnectionT& c )
  {
    ConnectorBase* p = new Connector<2, ConnectionT>(*this, c);
    delete this;
    return *p;
  }

  const ConnectionT*
  get_C() const
  {
    return C_;
  }

  size_t get_size() const{ return 1; }
};

// homogeneous connector containing >=K_CUTOFF entries
// specialization to define recursion termination for push_back
// internally use a normal vector to store elements
template < typename ConnectionT >
class Connector< K_CUTOFF, ConnectionT > : public vector_like<ConnectionT>
{
  std::vector< ConnectionT > C_;

public:
  Connector( const Connector< K_CUTOFF - 1, ConnectionT >& C, const ConnectionT& c )
    : C_( K_CUTOFF ) //, syn_id_(C.get_syn_id())
  {
    for ( size_t i = 0; i < K_CUTOFF - 1; i++ )
      C_[ i ] = C.get_C()[ i ];
    C_[ K_CUTOFF - 1 ] = c;
  }

  ~Connector()
  {
  }

  ConnectorBase& push_back( const ConnectionT& c )
  {
    C_.push_back( c );
    return *this;
  }

  const ConnectionT*
  get_C() const
  {
    return C_;
  }

  void
  send( event& e, thread t )
  {
    for ( size_t i = 0; i < C_.size(); i++ )
    {
      C_[ i ].send( e,
        ConnectorBase::get_t_lastspike());
    }
    ConnectorBase::set_t_lastspike( e.get_stamp().get_ms() );
  }

  size_t get_size() const{ return C_.size(); }
};

//removed template class specialization of connector class for simplicity

} // of namespace nest

#endif
