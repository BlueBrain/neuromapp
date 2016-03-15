#ifndef MAPP_GENERATOR_H
#define MAPP_GENERATOR_H

#include <vector>
#include <queue>
#include <utility>

#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"
#include "coreneuron_1.0/event_passing/queueing/queue.h"

namespace environment {

//forward declare
class sim_constraints;
class presyn_maker;


//events can be sent to other process, other thread or self
//create a pair out of event and event type
enum event_type{SPIKE,ITE,LOCAL};
typedef std::pair<queueing::event,event_type> gen_event;

class event_generator {
private:
    double cumulative_percents_[2];
    double sum_;
    std::vector<std::queue<gen_event> > event_pool_;

public:
    explicit event_generator(int nSpikes=100, int nIte=0, int nLocal=0);

    void operator()(
    int simtime, int ngroups, int rank, const presyn_maker& presyns);

    bool compare_top_lte(int id, double comparator) const;

    gen_event pop(int id);

    bool empty(int id) const { return event_pool_[id].empty(); }

    int get_size(int id) const { return event_pool_[id].size(); }

    double get_sum() const { return sum_; }
};

}// end of namespace

#endif
