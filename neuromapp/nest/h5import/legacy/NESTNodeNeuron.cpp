#include "NESTNodeNeuron.h"

NESTNodeNeuron::NESTNodeNeuron()
  : subnet_( 0 )
{
}
NESTNodeNeuron::NESTNodeNeuron( const int& subnet )
  : subnet_( subnet )
{
}

NESTNodeNeuron::~NESTNodeNeuron()
{
}