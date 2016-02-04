/*
 * Neuromapp - print.h, Copyright (c), 2015,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * timothee ewart - Swiss Federal Institute of technology in Lausanne,
 * sam.yates@epfl.ch (work)
 * timothee.ewart@epfl.ch (idea)
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
 * @file neuromapp/utils/mpi/print.h
 * \brief sequential print on the screen using the iosream class
 */


#ifndef MAPP_PRINT_
#define MAPP_PRINT_

#include <iostream>
#include <mpi.h>

namespace mapp{

    template <typename charT,typename traitsT=std::char_traits<charT> >
    struct basic_null_streambuf: std::basic_streambuf<charT,traitsT> {
    private:
        typedef typename std::basic_streambuf<charT,traitsT> streambuf_type;

    public:
        typedef typename streambuf_type::char_type char_type;
        typedef typename streambuf_type::int_type int_type;
        typedef typename streambuf_type::pos_type pos_type;
        typedef typename streambuf_type::off_type off_type;
        typedef typename streambuf_type::traits_type traits_type;

        virtual ~basic_null_streambuf() {}

    protected:
        virtual std::streamsize xsputn(const char_type *s,std::streamsize count) {
            return count;
        }

        virtual int_type overflow(char c) {
            return traits_type::not_eof(c);
        }
    };

    struct mask_stream {
        explicit mask_stream(bool mask_): mask(mask_) {}

        operator bool() const { return mask; }

        template <typename charT,typename traitsT>
        friend std::basic_ostream<charT,traitsT> &
        operator<<(std::basic_ostream<charT,traitsT> &O,const mask_stream &F) {
            int xindex=get_xindex();

            std::basic_streambuf<charT,traitsT> *saved_streambuf=
            static_cast<std::basic_streambuf<charT,traitsT> *>(O.pword(xindex));

            if (F.mask && saved_streambuf) {
                // re-enable by restoring saved streambuf
                O.pword(xindex)=0;
                O.rdbuf(saved_streambuf);
            }
            else if (!F.mask && !saved_streambuf) {
                // disable stream but save old streambuf
                O.pword(xindex)=O.rdbuf();
                O.rdbuf(get_null_streambuf<charT,traitsT>());
            }

            return O;
        }

    private:
        // our key for retrieve saved streambufs.

        static int get_xindex() {
            static int xindex=std::ios_base::xalloc();
            return xindex;
        }
        
        template <typename charT,typename traitsT>
        static std::basic_streambuf<charT,traitsT> *get_null_streambuf() {
            static basic_null_streambuf<charT,traitsT> the_null_streambuf;
            return &the_null_streambuf;
        }
        
        // true => do not filter
        bool mask;
    };

    // mask based on MPI rank
    inline mask_stream mpi_filter_master() {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        return mask_stream(rank==0);
    }

    // all rank normal version
    inline mask_stream mpi_filter_all() {
        return mask_stream(true);
    }

} // end namespace
#endif