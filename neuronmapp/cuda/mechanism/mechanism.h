#pragma once

struct mechanism{
    mechanism(size_t s, size_t l){
        _number_instance = s;
        _length_mechanism = l;
        _chuck_size = (s/128+1)*128;
 
        c.resize(chunk()*l);
        VEC_RHS.resize(chunk()*1);
        VEC_V.resize(chunk()*1);
        VEC_D.resize(chunk()*1);
        ppvar.resize(chunk()*3);
        ni.resize(chunk()*1);
        shadow_rhs.resize(chunk()*1);
        shadow_d.resize(chunk()*1);
        ion_data.resize(chunk()*3);

        std::iota(ni.begin(),ni.end(),0);
        std::size_t  n(0);
        std::generate(c.begin(),c.end(),[&](){return 3.14*n++;});
        //std::srand(1);
        //std::generate(c.begin(),c.end(),[&](){return ((double)std::rand() / (double)RAND_MAX) + (double)(std::rand()%10);});
        n = 0;
        std::generate(ppvar.begin(),ppvar.end(),[&](){return n++/chunk();});
    }
   
    virtual ~mechanism(){};
    virtual void state() = 0;
    virtual void current() = 0;

    std::size_t chunk() const{
        return _chuck_size; 
    }
 
    size_t number_instance() const{
        return _number_instance;
    }

    size_t _number_instance;
    size_t _length_mechanism;
    size_t _chuck_size;

    std::vector<double> c,VEC_RHS,VEC_V,VEC_D,shadow_rhs,shadow_d,ion_data;    
    std::vector<int> ni,ppvar;
};

#include "mechanism/Na.h"
#include "mechanism/lh.h"
#include "mechanism/ProbAMPANMDA_EMS.h"
