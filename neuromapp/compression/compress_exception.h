#ifndef compress_exception_h
#define compress_exception_h
#include <string>
#include <exception>

namespace neuromapp {

    // define these as valid aliases for the values 0-5
    class compression_exception : public std::exception {
        public:
            compression_exception(std::string message) throw() : message_{message} {}

            ~compression_exception() throw () {}

            virtual const char * what() const throw() { return message_.c_str() ; }

        private:
            std::string message_;
    };

    class zlib_exception : public compression_exception {
        public:
            zlib_exception(std::string message) : compression_exception(message) {}
    };

}
#endif
