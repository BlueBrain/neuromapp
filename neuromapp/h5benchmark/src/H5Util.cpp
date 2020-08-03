
#include <cstring>
#include "H5Util.hpp"

using namespace std;
using namespace h5benchmark;

fixedstring_t* vectorConv(vector<string> strings)
{
    const size_t  buffer_size = sizeof(fixedstring_t) * strings.size();
    fixedstring_t *converted  = (fixedstring_t *)malloc(buffer_size);
    off_t         count       = 0;
    
    for (auto it = strings.begin(); it != strings.end(); ++it)
    {
        strcpy(converted[count++], (*it).c_str());
    }
    
    return converted;
}

vector<string> bufferConv(fixedstring_t *strings, size_t count)
{
    vector<string> converted(count);
    
    for (off_t i = 0; i < count; i++)
    {
        converted[i] = string(strings[i]);
    }
    
    return converted;
}
