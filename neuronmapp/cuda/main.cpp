#include <iomanip>
#include <iostream>
#include <memory>
#include <algorithm>
#include <math.h>
#include <vector>
#include <sys/time.h>
#include <sched.h>
#include <chrono>
//#include <tuple> // to remove

#define DEBUG

#define START_LABEL(label) __asm__ ("#" #label "START" ::: "memory");
#define STOP_LABEL(label) __asm__ ("#" #label "STOP" ::: "memory");

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
        for(int i=0; i<v.size(); ++i)
            v[i] = new value_type(size,value_type::width);
    }

    ~benchmark(){
        std::for_each(v.begin(),v.end(),std::default_delete<mechanism>());
        v.clear();
    }

    void execute(){
        #pragma omp parallel for
        for(int i=0; i<v.size(); ++i)
            (v[i]->*ptf)();
    }
   
    std::chrono::duration<double> bench(){
        std::chrono::time_point<std::chrono::system_clock> start, end;       
        std::chrono::duration<double> elapsed_seconds;
      
        start = std::chrono::system_clock::now();
            execute();
        end = std::chrono::system_clock::now();
        elapsed_seconds = end-start;
      
        return elapsed_seconds; 
    } 

private:
    void (mechanism::*ptf)();
    std::vector<mechanism*> v;
};

template<class T>
void execution(std::size_t size){
    void (mechanism::*ptf[2])() = {&mechanism::state, &mechanism::current};
    const char * function_name[2] = {"state","current"};

    for(int i=0; i<1; ++i){
        benchmark<T> b(size, ptf[i]);
        std::chrono::duration<double>  time = b.bench();
        std::cout << "elapsed time, " << T::name() << " " << function_name[i] << ": "  << time.count() << "s\n";
    }
}

int main(int argc, char* argv[]){
    size_t size = atoi(argv[1]); // number of instance mechanism
    execution<Na>(size);
    //execution<lh>(size);
    //execution<ProbAMPANMDA_EMS>(size);
};

