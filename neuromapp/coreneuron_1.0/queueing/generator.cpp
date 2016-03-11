#include "coreneuron_1.0/queueing/generator.h"

event_generator::event_generator(int simTime, int nCells, int out, int nSpikes, int nLocal, int nIte){
    double sim_time_ = static_cast<double>(simTime);
    int cell_groups_ = nCells;

    double  sum = Spike + nIte + nLocal;
    //mean here the amount of time between spikes for entire sim
    double mean = sim_time_ / sum;
    lambda_ = 1.0 / (mean * num_cells_);
    cumulative_percents_[SPIKE] = num_spike_/sum;
    cumulative_percents_[ITE] = (num_spike_ + num_ite_)/sum;
}

void event_generator::operator(std::vector<int>& outputs){
    int dest = 0;
    int n = 0;
    int spike_c = 0;
    int ite_c = 0;
    int local_c = 0;
    double event_time = 0;
    int int_tt = 0;
    double percent = 0;
    event_type type;
    boost::mt19937 rng(rank_ + time(NULL));

    //generates random increment for event time
    //follows poisson process
    boost::random::exponential_distribution<double> time_g(lambda_);

    //generates indices for output presyns
    boost::random::uniform_int_distribution<> gid_g(0, (num_out_ - 1));

    //generates indices for threadDatas
    boost::random::uniform_int_distribution<> cellgroup_g(0, (cell_groups_ - 1));

    //generates percentages between 0 and 1
    boost::random::uniform_real_distribution<> percent_g(0.0,1.0);

    for(size_t i = 0; i < thread_datas_.size(); ++i){
        event_time = 0;
        //create events up until simulation end
        while(event_time < sim_time_){
            //increment event time
            double diff = time_g(rng);
            event_time += diff;
            if(event_time <= sim_time_){
                percent = percent_g(rng);
                //SPIKE EVENT
                if(percent < cumulative_percents_[SPIKE]){
                    type = SPIKE;
                    ++spike_c;
                    dest = outputs[gid_g(rng)];
                }
                //INTER THREAD EVENT
                else if(percent < cumulative_percents_[ITE]){
                    type = ITE;
                    ++ite_c;
                    dest = cellgroup_g(rng);
                    //dst cannot equal i
                    while(dest == i)
                        dest = cellgroup_g(rng);
                }
                //LOCAL EVENT
                else{
                    type = LOCAL;
                    ++local_c;
                    dest = i;//myID
                }
                //cast time to int and push into generated events array
                int_tt = static_cast<int>(event_time);
                event_pool_[i].push(dest, int_tt, type);
            }
        }
    }
    std::cout<<"spikes: "<<spike_c<<std::endl;
    std::cout<<"locals: "<<local_c<<std::endl;
    std::cout<<"ites: "<<ite_c<<std::endl;
}

bool compare_lte(int id, double comparator){
    assert(!event_pool_[id].empty());
    return (comparator <= event_pool_.front().first.t_);
}
