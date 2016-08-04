//#include "nmpi.h"
#include "NESTNodeNeuron.h"
#include "kernels.h"
#include "name.h"
#include "nest_datums.h"
#include "nest_types.h"
#include "tokenarray.h"
#include <map>
#include <omp.h>

using namespace nest;

class H5Neurons
{
private:
	NESTNeuronList neurons_;
	kernel_combi< float > kernel;
	std::string filename;
	std::vector< std::string > model_param_names;

	omp_lock_t tokenLock;

  GIDCollectionDatum CreateSubnets( const GIDCollectionDatum& added_neurons );
  GIDCollectionDatum CreateNeurons();


public:
  H5Neurons( const DictionaryDatum& din );
  void import( DictionaryDatum& dout );
  void addKernel( const std::string& name, TokenArray params );
};
