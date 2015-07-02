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

        for(size_t i=0; i < ni.size(); ++i)
		ni[i] = i;

        for(size_t i=0; i < c.size(); ++i)
		c[i] = 3.14*i;

        for(size_t i=0; i < ppvar.size(); ++i)
		ppvar[i] = i/chunk();
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
