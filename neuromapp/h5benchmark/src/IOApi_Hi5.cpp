
#include "highfive/H5DataSet.hpp"
#include "highfive/H5Group.hpp"
#include "highfive/H5DataSpace.hpp"
#include "boost/multi_array.hpp"
#include "IOApi_Hi5.hpp"

using namespace std;
using namespace boost;
using namespace HighFive;
using namespace h5benchmark;

IOApiH5::IOApiH5(std::string filename, bool enable_phdf5, bool enable_boost) :
    m_file(filename, File::ReadOnly,
           (enable_phdf5) ? MPIOFileDriver(MPI_COMM_WORLD, MPI_INFO_NULL) :
                            FileDriver()),
    m_boost_enabled(enable_boost)
{
    
}

IOApiH5::~IOApiH5()
{
    
}

int IOApiH5::readGroup(std::string name)
{
    // Retrieve the group and open the datasets
    Group   group       = m_file.getGroup(name);
    DataSet dset_struct = group.getDataSet("structure");
    DataSet dset_points = group.getDataSet("points");
    
    // Read the content available on each dataset
    if (m_boost_enabled)
    {
        boost::multi_array<int,   2> buffer_struct;
        boost::multi_array<float, 2> buffer_points;
        
        dset_struct.read(buffer_struct);
        dset_points.read(buffer_points);
    }
    else
    {
        vector<vector<int>>   buffer_struct;
        vector<vector<float>> buffer_points;
        
        dset_struct.read(buffer_struct);
        dset_points.read(buffer_points);
    }
    
    // printf ("Dataset: %s\n    (%f,%f,%f,%f)\n", name.c_str(),
    //                                             buffer_points[0][0],
    //                                             buffer_points[0][1],
    //                                             buffer_points[0][2],
    //                                             buffer_points[0][3]);
    
    return 0;
}
