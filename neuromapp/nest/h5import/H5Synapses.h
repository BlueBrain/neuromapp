#include <vector>
#include <omp.h>
#include <stdint.h>

#include "nest/h5import/NESTNodeSynapse.h"
#include "nest/h5import/h5reader.h"
#include "nest/h5import/kernels.h"

#ifndef H5Synapses_CLASS
#define H5Synapses_CLASS

namespace h5import
{

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
    std::string filename_;
    std::vector< std::string > model_params_;
    kernel_combi< double > kernel_;
    GIDCollectionDatum mapping_;

    uint64_t sizelimit_;
    uint64_t transfersize_;

    inline void singleConnect( NESTSynapseRef synapse, const size_t t_gid)
    {
        size_t source = synapse.source_neuron_;
        //std::vector<double>* values = kernel_( synapse.params_.begin(), synapse.params_.end() );

        std::vector<double> values(synapse.params_.size());
        for (int i=0; i<synapse.params_.size(); i++)
            values[i] = synapse.params_[i];
        kernel().connection_manager.connect(source, t_gid, values);
    }

  CommunicateSynapses_Status
       CommunicateSynapses( NESTSynapseList& synapses );
  void threadConnectNeurons( NESTSynapseList& synapses );
  void sort( NESTSynapseList& synapses );
  void integrateMapping( NESTSynapseList& synapses );
  void addKernel( std::string name, TokenArray params );


public:
  H5Synapses();
  void import();

  inline void set_filename(const std::string& path)
  {
      filename_ = path;
  }

  inline void set_properties(const std::vector<std::string>& prop_names)
  {
      model_params_ = prop_names;
  }

  inline void set_transfersize(const uint64_t& v)
  {
      transfersize_ = v;
  }

  inline void set_num_synapses(const uint64_t& v)
  {
      sizelimit_ = v;
  }

  inline void set_mapping(const GIDCollection& gids)
  {
      mapping_ = gids;
  }
};

};

#endif
