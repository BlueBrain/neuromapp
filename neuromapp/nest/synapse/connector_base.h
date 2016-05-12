/*
 *  connector_base.h
 *
 *  This file is part of NEST.
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
#include <vector>

#include "node.h"
#include "event.h"
#include "network.h"
#include "dictutils.h"
#include "spikecounter.h"
#include "nest_names.h"
#include "connector_model.h"
#include "connection_label.h"
#include "nest_datums.h"
#ifdef _OPENMP
#include <omp.h>
#endif

// when to truncate the recursive instantiation
#define K_CUTOFF 3

namespace nest
{

// base class to provide interface to decide
// - homogeneous connector (containing =1 synapse type)
//    -- which synapse type stored (syn_id)
// - heterogeneous connector (containing >1 synapse type)
class ConnectorBase
{

public:
  ConnectorBase();

  virtual void send( Event& e, thread t, const std::vector< ConnectorModel* >& cm ) = 0;

  virtual void trigger_update_weight( long_t vt_gid,
    thread t,
    const vector< spikecounter >& dopa_spikes,
    double_t t_trig,
    const std::vector< ConnectorModel* >& cm ) = 0;

  // destructor needed to delete connections
  virtual ~ConnectorBase(){};

  double_t
  get_t_lastspike() const
  {
    return t_lastspike_;
  }
  void
  set_t_lastspike( const double_t t_lastspike )
  {
    t_lastspike_ = t_lastspike;
  }

private:
  double_t t_lastspike_;
};

//Removed intermediate subclass, vector_like, for simplcity

// homogeneous connector containing K entries
template < size_t K, typename ConnectionT >
class Connector : public ConnectorBase
{
  ConnectionT C_[ K ];

public:
  Connector( const Connector< K - 1, ConnectionT >& Cm1, const ConnectionT& c )
  {
    for ( size_t i = 0; i < K - 1; i++ )
      C_[ i ] = Cm1.get_C()[ i ];
    C_[ K - 1 ] = c;
  }

  /**
   * Creates a new connector and remove the ith connection. To do so, the contents
   * of the original connector are copied into the new one. The copy is performed
   * in two parts, first up to the specified index and then the rest of the
   * connections after the specified index in order to
   * exclude the ith connection from the copy. As a result, returns a connector
   * with size K from a connector of size K+1.
   *
   * @param Cm1 the original connector
   * @param i the index of the connection to be deleted
   */
  Connector( const Connector< K + 1, ConnectionT >& Cm1, size_t i )
  {
    assert( i < K && i >= 0 );
    for ( size_t k = 0; k < i; k++ )
    {
      C_[ k ] = Cm1.get_C()[ k ];
    }

    for ( size_t k = i + 1; k < K + 1; k++ )
    {
      C_[ k - 1 ] = Cm1.get_C()[ k ];
    }
  }

  ~Connector()
  {
  }

  void
  send( Event& e, thread t, const std::vector< ConnectorModel* >& cm )
  {
    synindex syn_id = C_[ 0 ].get_syn_id();
    for ( size_t i = 0; i < K; i++ )
    {
      e.set_port( i );
      C_[ i ].send( e,
        t,
        ConnectorBase::get_t_lastspike(),
        static_cast< GenericConnectorModel< ConnectionT >* >( cm[ syn_id ] )
          ->get_common_properties() );
    }
    ConnectorBase::set_t_lastspike( e.get_stamp().get_ms() );
  }
};

//removed template class specialization of connector class for simplicity

} // of namespace nest

#endif
