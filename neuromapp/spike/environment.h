
#include "spike/spike_exchange.h"

#ifndef enviro_h
#define enviro_h

namespace spike {

struct environment {
private:
    int events_per_;
    int num_out_;
    int num_in_;
    int sim_time_;
    int num_procs_;
    int rank_;
    int total_received_;
    int total_relevent_;
    static const int min_delay_ = 5;

public:
    spike_vec generated_spikes_;
    spike_vec spikein_;
    spike_vec spikeout_;
    int_vec nin_;
    int_vec displ_;
    int_vec input_presyns_;
    int_vec output_presyns_;

    environment(int e, int o, int i, int s, int p, int r);

    void load_spikeout();

    void set_displ();

    bool matches(const spike_item& sitem);

    int all_matching();

    int received(){return total_received_;}
};

}
#endif
