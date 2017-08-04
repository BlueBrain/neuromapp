/*
 * Neuromapp - compress_exception.h, Copyright (c), 2015,
 * Devin Bayly - University of Arizona
 * baylyd@email.arizona.edu,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#ifndef compress_exception_h
#define compress_exception_h
#include <string>
#include <exception>

namespace neuromapp {

    // define these as valid aliases for the values 0-5
    class compression_exception : public std::exception {
        public:
            compression_exception(std::string message,int error_code) throw() : message_{message}, error_code_{error_code} {}

            ~compression_exception() throw () {}

            virtual const char * what() const throw() { return message_.c_str() ; }

            int get_error_code () {return error_code_;}

        private:
            std::string message_;
            int error_code_;
    };

    class zlib_exception : public compression_exception {
        public:
            zlib_exception(std::string message,int error_code ) : compression_exception(message,error_code) {}
    };

}
#endif
