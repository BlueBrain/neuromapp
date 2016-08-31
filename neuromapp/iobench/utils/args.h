/*
 * Neuromapp - args.h, Copyright (c), 2015,
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
 * @file neuromapp/iobench/utils/args.h
 * \brief basic shell for arguments
 */

#ifndef MAPP_IOBENCH_ARGS_H
#define MAPP_IOBENCH_ARGS_H

#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iostream>

// Get OMP header if available
#include "utils/omp/compatibility.h"

#ifdef IO_MPI
#include "utils/mpi/controler.h"
#endif

namespace iobench {

class args {

    private:
        int             procs_;
        int             rank_;
        unsigned int    threads_;
        unsigned int    npairs_;
        unsigned int    key_size_;
        unsigned int    value_size_;
        std::string     backend_;
        bool            read_;
        bool            write_;
        bool            async_;
        bool            compress_;
        bool            rnd_rd_;
        bool            rnd_wr_;
        unsigned int    niter_;
        unsigned int    skip_;




public:
    /** \fn argument(int argc = 0 , char * argv[] = NULL)
        \brief parse the command from argv and argv, the functor indicate the return type, I could
            write template and trait class, to do */
    explicit args(int argc = 0 , char * const argv[] = NULL) :
            procs_(-1), rank_(0), threads_(1), npairs_(1024), key_size_(32), value_size_(1024), backend_("map"),
            read_(true), write_(true), async_(false), compress_(false), rnd_rd_(true), rnd_wr_(true),
            niter_(1), skip_(0) {
        if (argc != 0) {
            std::vector<std::string> v(argv+1, argv+argc);
            argument_helper(v, "-b", backend(),to_string());
            argument_helper(v, "-n", npairs(),to_uint());
            argument_helper(v, "-i", niter(),to_uint());
            argument_helper(v, "-s", skip(),to_uint());
            argument_helper(v, "-k", keysize(),to_uint());
            argument_helper(v, "-v", valuesize(),to_uint());
            //argument_helper(v, "-a", async(),to_int());
            argument_helper(v, "-c", compress(),to_bool());
            argument_helper(v, "-r", read(),to_bool());
            argument_helper(v, "-w", write(),to_bool());
            argument_helper(v, "--rrd", rnd_rd(),to_bool());
            argument_helper(v, "--rwr", rnd_wr(),to_bool());

            niter_ += skip_;

        }

        // Get the number of processes from the MPI controller
#ifdef IO_MPI
        procs_ = mapp::master.size();
#else
        procs_ = -1;
#endif
        // Get the number of threads from OpenMP
        #pragma omp parallel
        {
            threads_ = omp_get_num_threads();
        }
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
        int operator()(std::string const& s){
            return std::atoi(s.c_str());
        }
    };

    struct to_uint{
        /** \fn operator()
         \brief functor that transform the argument to unsigned integer
         */
        int operator()(std::string const& s){
            return (unsigned int) std::atoi(s.c_str());
        }
    };

    struct to_bool{
        /** \fn operator()
         \brief functor that transform the argument to unsigned integer
         */
        bool operator()(std::string const& s){
            return std::atoi(s.c_str()) != 0;
        }
    };

    struct to_true{
        /** \fn operator()
         \brief functor that transform the return true if the looking argument is in the std::vector
         */
        bool operator()(std::string const& s){
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
    inline int rank() const {
        return rank_;
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
     \brief return compress data or not, read only
     */
    inline bool compress() const {
        return compress_;
    }

    /**
     \brief reading operation, read only
    */
    inline bool read() const {
        return read_;
    }

    /**
     \brief writing operation, read only
    */
    inline bool write() const {
        return write_;
    }

    /**
     \brief return the number of pairs, read only
     */
    inline int npairs() const {
        return npairs_;
    }

    /**
     \brief return the number of iterations, read only
     */
    inline int niter() const {
        return niter_;
    }

    /**
     \brief return the number of iterations to skip, read only
     */
    inline int skip() const {
        return skip_;
    }

    /**
     \brief random reading, read only
     */
    inline bool rnd_rd() const {
        return rnd_rd_;
    }

    /**
     \brief random writing, read only
     */
    inline bool rnd_wr() const {
        return rnd_wr_;
    }

    /**
     \brief return the key size, read only
     */
    inline unsigned int keysize() const {
        return key_size_;
    }

    /**
     \brief return the key size, read only
     */
    inline unsigned int valuesize() const {
        return value_size_;
    }



    /**
     \brief return the number of processors, write only
     */
    inline int &procs() {
        return procs_;
    }

    /**
    \brief return the rank ID, write only
    */
    inline int &rank() {
        return rank_;
    }

    /**
     \brief return the number of threads, write only
     */
    inline unsigned int &threads() {
        return threads_;
    }

    /**
     \brief return the name of the backend, write only
     */
    inline std::string &backend() {
        return backend_;
    }

    /**
     \brief return asynchronous mode or not, write only
     */
    inline bool &async() {
        return async_;
    }

    /**
     \brief return compress data or not, write only
     */
    inline bool &compress() {
        return compress_;
    }

    /**
     \brief reading operation, write only
    */
    inline bool &read() {
        return read_;
    }

    /**
     \brief writing operation, write only
    */
    inline bool &write() {
        return write_;
    }

    /**
     \brief return the number of pairs, write only
     */
    inline unsigned int &npairs() {
        return npairs_;
    }

    /**
     \brief return the number of iterations, write only
     */
    inline unsigned int &niter() {
        return niter_;
    }

    /**
     \brief return the number of iterations to skip, write only
     */
    inline unsigned int &skip() {
        return skip_;
    }

    /**
     \brief random reading, write only
     */
    inline bool &rnd_rd() {
        return rnd_rd_;
    }

    /**
     \brief random writing, write only
     */
    inline bool &rnd_wr() {
        return rnd_wr_;
    }

    /**
     \brief return the key size, write only
     */
    inline unsigned int &keysize() {
        return key_size_;
    }

    /**
     \brief return the key size, write only
     */
    inline unsigned int &valuesize() {
        return value_size_;
    }


    /** \brief the print function */
    void print(std::ostream& out) const{
        out << "IOBENCH configuration: \n"
                << "  npairs: " << npairs() << " \n"
                << "  key size: " << keysize() << " \n"
                << "  value size: " << valuesize() << " \n"
#ifdef IO_MPI
                << "  procs: " << procs() << " \n"
#else
                << "  procs: " << "OpenMP only" << " \n"
#endif
                << "  threads_: " << threads() << " \n"
                << "  backend_: " << backend() << " \n"
                << "  async_: " << async() << " \n"
                << "  compress_: " << compress() << " \n"
                << "  niter_: " << niter() << " \n"
                << "  skip_: " << skip() << " \n"
                << "  ops: ";

        if (read()) {
            out << "read " << ((rnd_rd() == true) ? "(random)" : "(sequential)") << " ";
        }
        if (write()) {
            out << "write " << ((rnd_wr() == true) ? "(random)" : "(sequential)") << " ";
        }
        out << "\n";
    }
};


/** \brief basic overload the ostream operator to print the arguments */
inline std::ostream &operator<<(std::ostream &out, args  const&  a){
     a.print(out);
     return out;
}

} //end namespace

#endif // MAPP_IOBENCH_ARGS_H
