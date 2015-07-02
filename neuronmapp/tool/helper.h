#pragma once

#include <omp.h>
#include <time.h>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/time.h>

#include "/usr/local/papi/5.4.1/include/papi.h"

enum event {vsu0,vsu1,flop,memory};

template<int n>
struct counter{};

template<>
struct counter<vsu0>{
    const static char* counters[4];
    const static int ntotal = 4;
};

template<>
struct counter<vsu1>{
    const static char* counters[4];
    const static int ntotal = 4;
};

template<>
struct counter<memory>{
    const static char* counters[2];
    const static int ntotal = 2;
};

template<>
struct counter<flop>{
    const static char* counters[1];
    const static int ntotal = 1;
};

const char* counter<vsu0>::counters[4] = {"PM_VSU0_1FLOP","PM_VSU0_2FLOP","PM_VSU0_4FLOP","PM_VSU0_8FLOP"};
const char* counter<vsu1>::counters[4] = {"PM_VSU1_1FLOP","PM_VSU1_2FLOP","PM_VSU1_4FLOP","PM_VSU1_8FLOP"};
const char* counter<memory>::counters[2] = {"PM_L3_PREF_ALL","PM_L3_CO_MEM"};
const char* counter<flop>::counters[1] = {"PM_FLOP"};

std::string error_message(int i){
    std::string message;
    switch(i){
        case PAPI_OK:
            message = "PAPI_OK";
            break;
        case PAPI_EINVAL:
            message = "PAPI_EINVAL";
            break;
        case PAPI_ESYS:
            message = "PAPI_ESYS";
            break;
        case PAPI_ENOEVST:
            message = "PAPI_ENOEVST";
            break;
        case PAPI_EISRUN:
            message = "PAPI_EISRUN";
            break;
        case PAPI_ENOTRUN:
            message = "PAPI_ENOTRUN";
            break;
        case PAPI_ECNFLCT:
            message = "PAPI_ECNFLCT";
            break;
        case PAPI_ENOEVNT:
            message = "PAPI_ENOEVNT";
            break;
        default:
            std::stringstream ss;
            ss << i;
            std::string str = ss.str();
            message = " I do not know " + str ;
            break;
    }
    return message;
}

struct error {
    error(int i, std::string in) {
        std::string message("Something went wrong in: ");
        message += in;
        if (i!=PAPI_OK) throw std::runtime_error(message + error_message(i));
    }

    error(int i, int j, std::string in) {
        std::string message("Something went wrong in: ");
        message += in;
        if (i!=j) throw std::runtime_error(message + error_message(i));
    }
};

// basic singleton, IT IS NOT GENERIC BLABLABLABLA
class helper_init{
public:
    helper_init(){
        if(!created){
            error(PAPI_library_init(PAPI_VER_CURRENT),PAPI_VER_CURRENT,"PAPI_library_init");
            error(PAPI_thread_init((long unsigned int (*)())omp_get_thread_num),"PAPI_thread_init");
            created = true;
        }
    }
protected:
    static bool created;
};
bool helper_init::created = false;

template<int n>
class recorder {
     public:

     recorder(size_t core_t, size_t smt_t) {
        core_thread = core_t;
        smt_thread = smt_t;
        hi = helper_init();
        totalthread = omp_get_max_threads();
        events.resize(totalthread);
        time_start.resize(totalthread);
        time_end.resize(totalthread);
        values.resize(totalthread);

        for(int i=0; i<totalthread; i++){
            values[i].resize(counter<n>::ntotal);
            for(auto it = values[i].begin(); it != values[i].end(); ++it)
                *it = 0;
            events[i] = PAPI_NULL;
        }

        for(int i = 0; i < counter<n>::ntotal; ++i)
            error(PAPI_event_name_to_code(const_cast<char*>(counter<n>::counters[i]), &(events_name[i])),"PAPI_event_name_to_code");
    }

    void clean(){
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            PAPI_cleanup_eventset(events[tid]);
            PAPI_destroy_eventset(&events[tid]);
        }
    }

    void start(){
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            error(PAPI_create_eventset(&events[tid]),"PAPI_create_eventset");
            error(PAPI_add_events(events[tid], events_name, counter<n>::ntotal),"PAPI_add_events");
            error(PAPI_start(events[tid]),"PAPI_start");
            time_start[tid] = std::chrono::system_clock::now();
        }
    }

    void stop(){
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            error(PAPI_stop(events[tid],&values[tid][0]),"PAPI_stop");
            time_end[tid] = std::chrono::system_clock::now();
        }
    }

    std::pair<double,double> flop(){
        double flops(0.);
        std::chrono::duration<double> time = std::chrono::duration<double>();
        for(int i=0; i < totalthread; ++i){
            flops += 2*static_cast<double>(values[i][0]*1); // factor 2 because 2 VSU, factor 1,2,4,8 = flops correction
            flops += 2*static_cast<double>(values[i][1]*2);
            flops += 2*static_cast<double>(values[i][2]*4);
            flops += 2*static_cast<double>(values[i][3]*8);
            time  += (time_end[i] - time_start[i]);
#ifdef NDEBUG
            std::cout << counter<n>::counters[0] << " " << values[i][0] << std::endl;
            std::cout << counter<n>::counters[1] << " " << values[i][1] << std::endl;
            std::cout << counter<n>::counters[2] << " " << values[i][2] << std::endl;
            std::cout << counter<n>::counters[3] << " " << values[i][3] << std::endl;
#endif
        }

        double t = time.count()/(double)totalthread;

        flops =  flops/(t*1000000000);
        return std::make_pair(flops,t);
    }

    std::pair<double,double> bandwidth(){
        double bw = double();
        std::chrono::duration<double> time = std::chrono::duration<double>();

        for(int i=0; i < totalthread; ++i){
            bw += static_cast<double>(values[i][0]);
            bw += static_cast<double>(values[i][1])/(2*smt_thread); // 2 given by IBM, totalthread from my works
            time  += (time_end[i] - time_start[i]);
#ifdef NDEBUG
            std::cout << counter<n>::counters[0] << " " << values[i][0] << std::endl;
            std::cout << counter<n>::counters[1] << " " << values[i][1] << std::endl;
#endif

        }
        bw *= 128; // 128 = cache line
        double t = time.count()/(double)totalthread;

        bw =  bw/(t*1000000000);

        return std::make_pair(bw,t);
    }

    std::pair<double,double> result() ;

    void binding_info(){
        #pragma omp parallel
        {
            #pragma omp critical
            std::cout << " cpu ID " << sched_getcpu() << " thread id " << omp_get_thread_num() <<  std::endl;
        }
    }

    private:
    helper_init hi;
    size_t totalthread;
    size_t smt_thread, core_thread;
    int events_name[counter<n>::ntotal];
    std::vector<std::vector<long long int> > values;
    std::vector<int> events;
    std::vector<std::chrono::time_point<std::chrono::system_clock> > time_start;
    std::vector<std::chrono::time_point<std::chrono::system_clock> > time_end;
};

template<>
std::pair<double,double> recorder<vsu0>::result() {
    return flop();
}

template<>
std::pair<double,double> recorder<vsu1>::result() {
    return flop();
}

template<>
std::pair<double,double> recorder<memory>::result() {
    return bandwidth();
}
