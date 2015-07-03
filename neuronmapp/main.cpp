#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cmath>

//#define NDEBUG

#include "mechanism/mechanism.h"
#include "mechanism/Na.h"
#include "mechanism/lh.h"
#include "mechanism/ProbAMPANMDA_EMS.h"


template<class T>
class benchmark{
    typedef T value_type;
public:
    benchmark(size_t size,void (mechanism::*pt)()):ptf(pt){
        v.resize(100);
        #pragma omp parallel for schedule(static)
        for(size_t i=0; i<v.size(); ++i)
            v[i] = new value_type(size,value_type::width);
    }

    ~benchmark(){
        for(size_t i=0; i<v.size(); ++i)
            delete v[i];
        v.clear();
    }

    void execute(){
        #pragma omp parallel for schedule(static)
        for(size_t i=0; i<v.size(); ++i)
            (v[i]->*ptf)();
    }

    double bench(){
        std::clock_t t_start = std::clock();
        execute();
        std::clock_t t_end = std::clock();
        return (t_end - t_start)/(double)CLOCKS_PER_SEC;
    }

private:
    void (mechanism::*ptf)();
    std::vector<mechanism*> v;
};

class result{
public:
    result(std::string n, double timer_state = 0, double timer_current = 0):name(n),time_state(timer_state),time_current(timer_current){ }

    void prsize_t(std::ostream & os) const{
        os << name << " time state " <<  time_state << " [s] " << "time current : " << time_current << " [s] " << std::endl;
    }

private:
    std::string name;
    double time_state, time_current;
};

std::ostream& operator<<(std::ostream& os, result const& a){
    a.prsize_t(os);
    return os;
}

template<class T> // T is the mechanism
void execution(std::size_t size){
    double time_state(0);
    double time_current(0);
    benchmark<T> b1(size,&mechanism::state);
    time_state =  b1.bench();
    benchmark<T> b2(size,&mechanism::current);
    time_current =  b2.bench();
    result r(T::name(),time_state,time_current);
    std::cout << r << std::endl;
}

int main(int  argc, char* argv[]){
    size_t size = atoi(argv[1]); // number of instance mechanism
    execution<Na>(size);
    execution<lh>(size);
    execution<ProbAMPANMDA_EMS>(size);
};
