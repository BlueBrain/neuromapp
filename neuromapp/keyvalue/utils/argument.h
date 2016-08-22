/*
 * Neuromapp - arguments.h, Copyright (c), 2015,
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
 * @file neuromapp/keyvalue/utils/arguments.h
 * \brief basic shell for arguments
 */

#ifndef MAPP_ARGUEMENTS_H
#define MAPP_ARGUEMENTS_H

#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#ifdef _OPENMP
   #include <omp.h>
#else
   #define omp_get_num_threads() 1
#endif

#include "utils/mpi/controler.h"

namespace keyvalue {

class argument {
private:
    int         procs_;
    int         threads_;
    std::string backend_;
    bool        async_;
    bool        flash_;
    int         usecase_;
    bool        task_deps_;
    float       st_;
    float       md_;
    float       dt_;
    int         cg_;
    int         voltages_size_;

public:
    /** \fn argument(int argc = 0 , char * argv[] = NULL)
        \brief parse the command from argv and argv, the functor indicate the return type, I could
            write template and trait class, to do */
    explicit argument(int argc = 0 , char * const argv[] = NULL) :
            procs_(1), threads_(1), backend_("map"), async_(false),
            flash_(false), usecase_(1), task_deps_(false), st_(1.), md_(0.1),
            dt_(0.025), cg_(1){
        if(argc != 0){
            std::vector<std::string> v(argv+1, argv+argc);
            argument_helper(v,"-b",backend(),to_string());
            argument_helper(v,"-st",st(),to_double());
            argument_helper(v,"-md",md(),to_double());
            argument_helper(v,"-dt",dt(),to_double());
            argument_helper(v,"-cg",cg(),to_int());
            argument_helper(v,"-uc",usecase(),to_int());
            argument_helper(v,"-a",async(),to_true());
            argument_helper(v,"-f",flash(),to_true());
            argument_helper(v,"-d",taskdeps(),to_true());
        }
        // Get the number of processes from the MPI controller
        procs_ = mapp::master.size();
        // Get the number of threads from OpenMP
        #pragma omp parallel
        {
            threads_ = omp_get_num_threads();
        }
        // Adjust voltages_size_ according to the use case
        voltages_size_ = usecase_*4096/2.5*350;
    }

    struct to_string{
            /** \fn operator()
         \brief functor that does nothing because argv is transformed into std::string
             */
            std::string operator()(std::string const& s){
                return s;
            }
    };

    struct to_double{
        /** \fn operator()
         \brief functor that transform the argument to double
         */
        double operator()(std::string const& s){
            return std::atof(s.c_str());
        }
    };

    struct to_int{
        /** \fn operator()
         \brief functor that transform the argument to integer
         */
        double operator()(std::string const& s){
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
        //else
        //    std::cout << "Ignoring invalid parameter: \n ";
    }

    /**
     \brief return the voltage size, read only
    */
    inline int voltages_size() const{
        return voltages_size_;
    }

    /**
    \brief return the number of processors, read only
    */
    inline int procs() const {
        return procs_;
    }

    /**
     \brief return the number of threads, read only
     */
    inline int threads() const {
        return threads_;
    }

    /**
     \brief return the name of the backend, read only
     */
    inline std::string backend() const {
        return backend_;
    }

    /**
     \brief return asynchronous mode or not, read only
     */
    inline bool async() const {
        return async_;
    }

    /**
     \brief using flash or not, read only
    */
    inline bool flash() const {
        return flash_;
    }

    /**
     \brief return the use case, read only
     */
    inline int usecase() const{
        return usecase_;
    }

    /**
     \brief using task dependency implementation or not, read only
     */
    inline bool taskdeps() const {
        return task_deps_;
    }

    /**
     \brief return the simulation time, read only
     */
    inline float st() const{
        return st_;
    }

    /**
     \brief return the minimum delay, read only
     */
    inline float md() const{
        return md_;
    }

    /**
     \brief return the step time, read only
     */
    inline float dt() const{
        return dt_;
    }

    /**
     \brief return the number of cell group, read only
     */
    inline int cg() const{
        return cg_;
    }

    /**
     \brief return the volage size, write only
     */
    inline int &voltages_size(){
        return voltages_size_;
    }

    /**
     \brief return the number of processors, write only
     */
    inline int &procs(){
        return procs_;
    }

    /**
     \brief return the number of threads, write only
     */
    inline int &threads(){
        return threads_;
    }

    /**
     \brief return the name of the backend, write only
     */
    inline std::string &backend(){
        return backend_;
    }

    /**
     \brief return asynchronous mode or not, read only, write only
     */
    inline bool &async(){
        return async_;
    }

    /**
     \brief using flash or not, write only
     */
    inline bool &flash(){
        return flash_;
    }

    /**
     \brief return the use case, write only
     */
    inline int &usecase(){
        return usecase_;
    }

    /**
     \brief using task dependency implementation or not, write only
     */
    inline bool &taskdeps(){
        return task_deps_;
    }

    /**
     \brief return the simulation time, write only
     */
    inline float &st(){
        return st_;
    }

    /**
     \brief return the minimum delay, write only
     */
    inline float &md(){
        return md_;
    }

    /**
     \brief return the step time, write only
     */
    inline float &dt(){
        return dt_;
    }

    /**
     \brief return the number of cell group, write only
     */
    inline int &cg(){
        return cg_;
    }

    /** \brief the print function, I do not like friend function */
    void print(std::ostream& out) const{
        out << " voltages_size_: " << voltages_size() << " \n"
            << " procs: " << procs() << " \n"
            << " threads_: " << threads() << " \n"
            << " backend_: " << backend() << " \n"
            << " async_: " << async() << " \n"
            << " flash_: " << flash() << " \n"
            << " usecase_: " << usecase() << " \n"
            << " OpenMP implementation: " << ((taskdeps() == true) ? "task dependency" : "for loop") << " \n"
            << " st_: " << st() << " \n"
            << " md_: " << md() << " \n"
            << " dt_: " << dt() << " \n"
            << " cg_: " << cg() << " \n";
    }
};


/** \brief basic overload the ostream operator to print the arguement */
inline std::ostream &operator<<(std::ostream &out, argument  const&  a){
     a.print(out);
     return out;
}

} //end namespace

#endif /* argument_h */
