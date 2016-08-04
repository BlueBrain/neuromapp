#include <deque>
#include <vector>
//#include "nmpi.h"
#include "NESTNodeSynapse.h"
#include <map>

#include <omp.h>

//#include "H5SynMEMPedictor.h"
#include "H5SynapseLoader.h"
#include "kernels.h"

#ifndef H5Synapses_CLASS
#define H5Synapses_CLASS

enum CommunicateSynapses_Status
{
  NOCOM,
  SEND,
  RECV,
  SENDRECV,
  UNSET
};

/**
 * H5Synapses - load Synapses from HDF5 and distribute to nodes
 *
 */

class H5Synapses
{
private:
  omp_lock_t tokenLock_;

  std::vector< std::string > model_params_;
  long stride_;
  kernel_combi< double > kernel_;
  GIDCollectionDatum mapping_;
  NESTSynapseList synapses_;

  size_t synmodel_id_;

    std::string filename_;

  long num_syanpses_per_process_;
  long last_total_synapse_;

  void singleConnect( NESTSynapseRef synapse,
          const nest::index t_gid,
    uint64_t& n_conSynapses );

  uint64_t threadConnectNeurons( uint64_t& n_conSynapses );

  void freeSynapses();
  CommunicateSynapses_Status CommunicateSynapses();

  void sort();
  void integrateMapping();


  void addKernel( std::string name, TokenArray params );

public:
  H5Synapses();
  ~H5Synapses();
  void import();

  //void set_status( const DictionaryDatum& din );
  void set_filename(const std::string& path);
  void set_properties(const std::vector<std::string>& prop_names);
  void set_mapping(const std::vector<long> gids);
};

#endif
