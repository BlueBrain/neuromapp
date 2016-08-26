#include "NESTNodeSynapse.h"
//#include "nmpi.h"

/*NESTNodeSynapse::NESTNodeSynapse()
{}
NESTNodeSynapse::NESTNodeSynapse(const unsigned int& source_neuron, const
unsigned int& target_neuron)
{
  set(source_neuron, target_neuron);
}
NESTNodeSynapse::~NESTNodeSynapse()
{}
/*void NESTNodeSynapse::set(const unsigned int& source_neuron, const unsigned
int& target_neuron)
{
  source_neuron_ = source_neuron;
  target_neuron_ = target_neuron;

  const nest::index vp = nest::kernel().vp_manager.suggest_vp(target_neuron_);
  node_id_  = nest::kernel().mpi_manager.get_process_id(vp);

}
void NESTNodeSynapse::integrateMapping(const GIDCollection& gidc)
{
  source_neuron_ = gidc[source_neuron_];
  target_neuron_ = gidc[target_neuron_];

  const nest::index vp = nest::kernel().vp_manager.suggest_vp(target_neuron_);
  node_id_  = nest::kernel().mpi_manager.get_process_id(vp);
}
void NESTNodeSynapse::serialize(unsigned int* buf)
{
  buf[0] = source_neuron_;
  buf[1] = target_neuron_;
  buf[2] = node_id_;
  memcpy(&buf[3], &prop_values_[0], 5*sizeof(double));
}
void NESTNodeSynapse::deserialize(unsigned int* buf)
{
  source_neuron_ = buf[0];
  target_neuron_ = buf[1];
  node_id_ = buf[2];

  memcpy(&prop_values_[0], &buf[3], 5*sizeof(double));
}
bool NESTNodeSynapse::operator<(const NESTNodeSynapse& rhs) const
{
  return node_id_ < rhs.node_id_;
}*/
