#ifndef MAPP_GENERATOR_H
#define MAPP_GENERATOR_H

namespace environment {

//events can be sent to other process, other thread or self
//create a pair out of event and event type
enum event_type{SPIKE,ITE,LOCAL};
typedef std::pair<event,event_type> gen_event;

class event_generator {
private:
    int cumulative_percents_[2];
    int sum_;
    std::vector<std::queue<gen_event> > event_pool_;

public:
    event_generator();

    void operator()(const sim_env& env, const presyn_maker& presyns);

    bool compare_lte const (int id, double comparator);

    gen_event pop(int id) { return event_pool_[id].pop(); }

    bool empty(int id) const { return event_pool_[id].empty(); }
};

}// end of namespace

#endif
