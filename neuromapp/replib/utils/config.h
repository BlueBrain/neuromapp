/*
 * Neuromapp - config.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/replib/utils/config.h
 * \brief basic shell for benchmark configuration
 */

#ifndef MAPP_CONFIG_H
#define MAPP_CONFIG_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "utils/mpi/controler.h"

namespace replib {

class config {
private:
    int         procs_;
    int         id_;
    std::string backend_;
    std::string write_;
    std::string input_dist_;
    std::string output_report_;
    bool        invert_;
    int         numcells_;
    int         sim_steps_;
    int         rep_steps_;
    int         elems_per_step_;
    int         sim_time_ms_;
    bool        check_;
    bool        passed_;

public:
    /** \fn config(int argc = 0 , char * argv[] = NULL)
        \brief parse the command from argc and argv, the functor indicates the
            return type, I could write template and trait class, to do */
    explicit config (int argc = 0 , char * const argv[] = NULL) :
            procs_(1), backend_("mpiio"), write_("rnd1b"), input_dist_(""),
            output_report_(""), invert_(false), numcells_(10), sim_steps_(15),
            rep_steps_(1), elems_per_step_(0), sim_time_ms_(100), check_(false),
            passed_(false) {
        if (argc != 0) {
            std::vector<std::string> v(argv+1, argv+argc);
            argument_helper(v,"-b",backend(),to_string());
            argument_helper(v,"-w",write(),to_string());
            argument_helper(v,"-o",output_report(),to_string());
            argument_helper(v,"-f",input_dist(),to_string());
            argument_helper(v,"-i",invert(),to_true());
            argument_helper(v,"-c",numcells(),to_int());
            argument_helper(v,"-s",sim_steps(),to_int());
            argument_helper(v,"-r",rep_steps(),to_int());
            argument_helper(v,"-t",sim_time_ms(),to_int());
            argument_helper(v,"-v",check(),to_true());
        }
        // Get the number of processes and rank ID from the MPI controller
        procs_ = mapp::master.size();
        id_ = mapp::master.rank();
    }

    struct to_string{
            /** \fn operator()
         \brief functor that does nothing because argv is transformed into std::string
             */
            std::string operator()(std::string const& s){
                return s;
            }
    };

    struct to_int{
        /** \fn operator()
         \brief functor that transform the argument to integer
         */
        int operator()(std::string const& s){
            return std::atoi(s.c_str());
        }
    };

    struct to_true{
        /** \fn operator()
         \brief functor that transform the return true if the looking argument is in the std::vector
         */
        double operator()(std::string const& s){
            return true;
        }
    };

    /** \fn argument_helper(std::vector<std::string> const& v, std::string const& s, F& function, BinaryOperation op)
     \brief extract the different flags of the command line and fill up the data member, the function looks for the
     wanted argument (in a vector) and extracts the next element, using a binary operation associated to the write function
     and a functor (type dependent) */
    template<class F, class UnaryOperation>
    void argument_helper(std::vector<std::string> const& v, std::string const& s, F& function, UnaryOperation op){
        std::vector<std::string>::const_iterator it;
        it = find(v.begin(), v.end(), s);
        if (it != v.end())
            function = op(*(it+1));
    }

    /**
    \brief return the number of processors, read only
    */
    inline int procs() const {
        return procs_;
    }

    /**
     \brief return the rank ID, read only
     */
    inline int id() const {
        return id_;
    }

    /**
     \brief return the I/O backend, read only
    */
    inline std::string backend() const {
        return backend_;
    }

    /**
     \brief return the write distribution, read only
    */
    inline std::string write() const {
        return write_;
    }

    /**
     \brief return the path to the file distribution, read only
     */
    inline std::string input_dist() const {
        return input_dist_;
    }

    /**
     \brief return the path to the output report, read only
     */
    inline std::string output_report() const {
        return output_report_;
    }

    /**
     \brief return whether rank IDs are inverted or not, read only
     */
    inline bool invert() const {
        return invert_;
    }

    /**
     \brief return the number of cells, read only
     */
    inline int numcells() const {
        return numcells_;
    }

    /**
     \brief return the number of simulation steps, read only
     */
    inline int sim_steps() const {
        return sim_steps_;
    }

    /**
     \brief return the number of reporting steps, read only
     */
    inline int rep_steps() const {
        return rep_steps_;
    }

    /**
     \brief return the number of elements per reporting step, read only
     */
    inline int elems_per_step() const {
        return elems_per_step_;
    }

    /**
     \brief return the amount of time spent in each simulation iteration (in ms), read only
     */
    inline int sim_time_ms() const {
        return sim_time_ms_;
    }

    /**
     \brief return whether output report should be checked for correctness or not, read only
     */
    inline bool check() const {
        return check_;
    }

    /**
     \brief return whether output report verification passed or failed, read only
     */
    inline bool passed() const {
        return passed_;
    }

    /**
     \brief return the number of processors, write only
     */
    inline int& procs() {
        return procs_;
    }

    /**
     \brief return the rank ID, write only
     */
    inline int& id() {
        return id_;
    }

    /**
      \brief return the I/O backend, write only
     */
     inline std::string& backend() {
         return backend_;
     }

    /**
      \brief return the write distribution, write only
     */
     inline std::string& write() {
         return write_;
     }

     /**
      \brief return the path to the file distribution, write only
      */
     inline std::string& input_dist() {
         return input_dist_;
     }

     /**
      \brief return the path to the output report, write only
      */
     inline std::string& output_report() {
         return output_report_;
     }

     /**
      \brief return whether rank IDs are inverted or not, write only
      */
     inline bool& invert() {
         return invert_;
     }

     /**
      \brief return the number of cells, write only
      */
     inline int& numcells() {
         return numcells_;
     }

     /**
      \brief return the number of simulation steps, write only
      */
     inline int& sim_steps() {
         return sim_steps_;
     }

     /**
      \brief return the number of reporting steps, write only
      */
     inline int& rep_steps() {
         return rep_steps_;
     }

     /**
      \brief return the number of elements per reporting step, write only
      */
     inline int& elems_per_step() {
         return elems_per_step_;
     }

     /**
      \brief return the amount of time spent in each simulation iteration (in ms), write only
      */
     inline int& sim_time_ms() {
         return sim_time_ms_;
     }

     /**
      \brief return whether output report should be checked for correctness or not, write only
      */
     inline bool& check() {
         return check_;
     }

     /**
      \brief return whether output report verification passed or failed, write only
      */
     inline bool& passed() {
         return passed_;
     }

    /** \brief the print function, I do not like friend function */
    void print(std::ostream& out) const {
        out << " procs: " << procs() << " \n"
            << " backend_: " << backend() << " \n"
            << " write_: " << write() << " \n"
            << " input_dist_: " << input_dist() << " \n"
            << " output_report_: " << output_report() << " \n"
            << " invert_: " << invert() << " \n"
            << " numcells_: " << numcells() << " \n"
            << " sim_steps_: " << sim_steps() << " \n"
            << " rep_steps_: " << rep_steps() << " \n"
            << " elems_per_step_: " << elems_per_step() << " \n"
            << " sim_time_ms_: " << sim_time_ms() << " \n"
            << " check_: " << check() << " \n";
        if (check()) {
            out << "Report verification: " << ( passed() ? "PASSED" : "FAILED") << "\n";
        }
    }
};


/** \brief basic overload the ostream operator to print the config class */
inline std::ostream &operator<<(std::ostream &out, config  const& c) {
     c.print(out);
     return out;
}

} //end namespace

#endif /* config_h */
