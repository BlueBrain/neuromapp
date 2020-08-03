
#include <morphio/errorMessages.h>
#include "IOApi_MKit.hpp"

using namespace morphokit;
using namespace h5benchmark;

IOApiMKit::IOApiMKit(std::string filename, bool enable_phdf5) :
    m_file(dynamic_pointer_cast<FileStorage>(storage(filename)))
{
    // Disable any warnings by MorphIO
    morphio::set_maximum_warnings(0);
}

~IOApiMKit::IOApiMKit()
{
    
}

int IOApiMKit::readGroup(std::string name)
{
    // Retrieving the morphology is enough to read the whole group
    Morphology group = m_file->get(name);
    
    return 0;
}
