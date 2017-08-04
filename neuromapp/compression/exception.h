/*
 * Neuromapp - exception.h, Copyright (c), 2015,
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
//
//  exception.h
//  learning_engine
//
//  Created by Tim Ewart on 21/11/2016.
//
//

#ifndef exception_h
#define exception_h

#include <exception>
#include <string>

namespace neuromapp {

enum lengine_error {
    NEUROMAPP_OK = 0,
    NEUROMAPP_BAD_ARG,
    NEUROMAPP_BAD_USAGE,
    NEUROMAPP_BAD_DATA,
    NEUROMAPP_BAD_THREAD,
    NEUROMAPP_UNKNOWN_ERROR
};

class learning_engine_exception : public std::exception {
  public:
    learning_engine_exception(std::string message, int error_code = NEUROMAPP_OK) throw()
        : message_(message), error_code_(error_code) {}
    ~learning_engine_exception() throw() {}

    virtual const char *what() const throw() { return message_.c_str(); }

    int get_error_code() const { return error_code_; }

  private:
    std::string message_;
    int error_code_;
};

template <lengine_error error>
inline bool is_bad(learning_engine_exception const &ex) {
    return ex.get_error_code() == error;
}

class cpu_memory_allocation : public learning_engine_exception {
  public:
    cpu_memory_allocation(std::string message, int error_code) : learning_engine_exception(message, error_code) {}
};

class gpu_memory_allocation : public learning_engine_exception {
  public:
    gpu_memory_allocation(std::string message, int error_code) : learning_engine_exception(message, error_code) {}
};


} // namespace lengine

#endif /* exception_h */
