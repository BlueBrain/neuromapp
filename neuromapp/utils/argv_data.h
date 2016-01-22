/*
 * Neuromapp - argv_data.h, Copyright (c), 2015,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * sam.yates@epfl.ch
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/utils/argv_data.h
 *  \brief encapuslate and construct argv form the command line for C miniapp
 */

#ifndef MAPP_ARGV_DATA_
#define MAPP_ARGV_DATA_

#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

namespace mapp {

/** @brief Container for argc, argv data.*/

struct argv_data {
    /** Default constructor.
     *
     * Represents empty argument list, with argc=0 and argv[0]==0.
     */

    argv_data(): argv_(1,(char *)0) {}

    /** Construct argument data from iterator range.
     *
     * @tparam I Iterator type; must be able to construct std::string from corresponding value_type.
     * @param b Beginning of range
     * @param e End of range
     */
    template <typename I>
    argv_data(I b,I e) {
        std::vector<int> offsets;

        while (b!=e) {
            offsets.push_back((int)argv_buf.size());
            std::string arg(*b++);
            std::copy(arg.begin(),arg.end(),std::back_inserter(argv_buf));
            argv_buf.push_back('\0');
        }

        for (std::vector<int>::const_iterator i=offsets.begin();i!=offsets.end();++i)
            argv_.push_back(&argv_buf[*i]);

        argv_.push_back((char *)0);
    }

    /** Corresponding argc value. */
    int argc() const { return (int)argv_.size()-1; }

    /** Corresponding argv value. */
    char * const *argv() { return &argv_[0]; }
    
    /** Verify consistency */
    void check_invariants() {
        int argc=this->argc();
        char * const *argv=this->argv();

        // argc >= 0.
        if (argc<0) throw std::logic_error("argv_data: negative argc");

        // argv_.size()==argc+1.
        if (argv_.size()!=1+argc) throw std::logic_error("argv_data: argv_ vector size mismatch");

        // argv[i] should be non-zero for i=0..(argc-1), and point to within argv_buf.
        char *argv_buf_begin=&argv_buf[0];
        char *argv_buf_end=argv_buf_begin+argv_buf.size();

        for (int i=0;i<argc;++i) {
            if (!argv[i]) throw std::logic_error("argv_data: zero argv[i] entry for i<argc");

            // NB: strictly speaking, if argv[i] does not point to a character within the buffer,
            // the following comparisons constitue undefined-behaviour.
            if (argv[i]<argv_buf_begin || argv[i]>=argv_buf_end)
                throw std::logic_error("argv_data: invalid argv entry");
        }
    }

private:
    std::vector<char> argv_buf;
    std::vector<char *> argv_;
};

}

#endif