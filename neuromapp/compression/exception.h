/* Filename : exception.h
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, Blue Brain Project
 * Purpose : xxx
 * Date : 2017-08-01 
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
        /**
        * error_code_ 
        *
        *
        * @brief
        *
        * @param error_code
        *
        * @return message_(message),
        */
        : message_(message), error_code_(error_code) {}
        /**
        * throw 
        *
        *
        * @brief
        *
        * @param 
        *
        * @return learning_engine_exception()
        */
    ~learning_engine_exception() throw() {}

        /**
        * throw 
        *
        *
        * @brief
        *
        * @param 
        *
        * @return virtual const char *what() const
        */
    virtual const char *what() const throw() { return message_.c_str(); }

    int get_error_code() const { return error_code_; }

  private:
    std::string message_;
    int error_code_;
};

template <lengine_error error>
        /**
        * is_bad 
        *
        *
        * @brief
        *
        * @param learning_engine_exception const &ex
        *
        * @return inline bool
        */
inline bool is_bad(learning_engine_exception const &ex) {
    return ex.get_error_code() == error;
}

class cpu_memory_allocation : public learning_engine_exception {
  public:
        /**
        * learning_engine_exception 
        *
        *
        * @brief
        *
        * @param message, error_code
        *
        * @return cpu_memory_allocation(std::string message, int error_code) :
        */
    cpu_memory_allocation(std::string message, int error_code) : learning_engine_exception(message, error_code) {}
};

class gpu_memory_allocation : public learning_engine_exception {
  public:
        /**
        * learning_engine_exception 
        *
        *
        * @brief
        *
        * @param message, error_code
        *
        * @return gpu_memory_allocation(std::string message, int error_code) :
        */
    gpu_memory_allocation(std::string message, int error_code) : learning_engine_exception(message, error_code) {}
};


} // namespace lengine

#endif /* exception_h */
