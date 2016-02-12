#ifndef enviro_h
#define enviro_h
#include <stdlib.h>
#include <stddef.h>
#include <vector>

#include "spike/algos.hpp"

namespace spike {

typedef std::vector<int> int_vec;
typedef std::vector<spike_item> spike_vec;

struct environment {
private:
    int events_per_;
    int num_out_;
    int num_in_;
    int num_procs_;
    int rank_;
    int total_received_;
    int total_relevent_;
    static const int cell_groups_ = 1;
    static const int min_delay_ = 5;

public:
    spike_vec generated_spikes_;
    spike_vec spikein_;
    spike_vec spikeout_;
    int_vec nin_;
    int_vec displ_;
    int_vec input_presyns_;
    int_vec output_presyns_;

    environment(int e, int o, int i, int p, int r);

    void time_step();

    void set_displ();

    void generate_all_events(int totalTime);

    bool matches(const spike_item& sitem);

    int filter();

    int mindelay(){return min_delay_;}
    int cells(){return cell_groups_;}
    int received(){return total_received_;}
    int relevent(){return total_relevent_;}
    void increment_time(){}
};

}
#endif
