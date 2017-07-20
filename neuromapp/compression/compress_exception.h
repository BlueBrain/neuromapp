/* Filename : compress_exception.h
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, EPFL
 * Purpose : xxx
 * Date : 2017-07-20 
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

            int const get_error_code () {return error_code_;}

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
