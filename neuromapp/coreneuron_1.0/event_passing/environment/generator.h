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

/** event_generator
 *  /brief generates all events needed for the simulation and stores them
 *  in a queue per cell group. These events can be retrieved using
 *  the pop function.
 */
class event_generator {
private:
    double cumulative_percents_[2];
    double sum_;
    std::vector<std::queue<gen_event> > event_pool_;

public:
    /** \fn event_generator(int nSpike, int nIte, int nLocal)
     *  \brief creates the generator and establishes the percentages
     *  of each event type (spike, inter-thread event, local)
     *  \param nSpike number of spike events
     *  \param nIte number of inter thread events
     *  \param nLocal number of local events
     */
    explicit event_generator(int nSpike=100, int nIte=0, int nLocal=0);

    /** \fn void operator()(simtime, ngroups, rank, presyns)
     *  \brief "generate" function for the generator. Creates all the events.
     *  \param simtime the total time of the simulation
     *  \param ngroups the number of cell groups
     *  \param rank the rank of the current process (for a unique random seed)
     *  \param presyns used to create spike events (need output gid info)
     */
    void operator()(
    int simtime, int ngroups, int rank, const presyn_maker& presyns);


    /** \fn gen_event pop()(int id)
     *  \brief retrieves the top most element from the specified queue
     *  \param id specifies which queue to pop from
     *  \return the top most element of the queue
     *  \postcond the top most element is removed from the queue
     */
    gen_event pop(int id);

//GETTERS
    /** \fn compare_top_lte(int id, double comparator)
     *  \brief compares the top item from the ith queue against the comparator.
     *  \param id (cell group ID) determines which queue
     *  \return true if top <= comparator. Else false
     */
    bool compare_top_lte(int id, double comparator) const;

    /** \fn empty(int id)
     *  \param id (cell group ID) determines which queue
     *  \return if the queue is empty
     */
    bool empty(int id) const { return event_pool_[id].empty(); }

    /** \fn get_size(int id)
     *  \param id (cell group ID) determines which queue
     *  \return size of the ith queue
     */
    int get_size(int id) const { return event_pool_[id].size(); }

    /** \fn get_sum()
     *  \return sum of all events
     */
    double get_sum() const { return sum_; }
};

}// end of namespace

#endif
