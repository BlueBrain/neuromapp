#ifndef MAPP_GENERATOR_H
#define MAPP_GENERATOR_H

#include <vector>
#include <queue>
#include <utility>

#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"
#include "coreneuron_1.0/event_passing/queueing/queue.h"

namespace environment {

//create a pair out of time and presyn
typedef std::pair<int, double> gen_event;

/** event_generator
 *  /brief generates all events needed for the simulation and stores them
 *  in a queue per cell group. These events can be retrieved using
 *  the pop function.
 */
class event_generator {
private:
    std::vector<std::queue<gen_event> > event_pool_;

public:

    /** \fn event_generator(int nSpike, int simtime, int ngroups,
     *      int rank, int nprocs, int ncells)
     *  \brief the generator constructor. Creates all the events.
     *  \param nspikes the total number of spikes created in the simulation
     *  \param simtime the total time of the simulation
     *  \param ngroups the number of cell groups
     *  \param rank the rank of the current process (for a unique random seed)
     *  \param nprocs the number of processes in the simulation
     *  \param ncells the total number of cells
     */
    event_generator(int nSpikes, int simtime, int ngroups,
    int rank, int nprocs, int ncells);

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
};

}// end of namespace

#endif
