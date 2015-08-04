#pragma once

#include <exception>
#include <string>

#include "utils/error.h"


namespace mapp{

    class driver_exception : public std::exception {
        public:
            explicit driver_exception(const std::string& m = std::string(), int error=MAPP_OK): error_code(error),
                                                                                                message(m){}
            explicit driver_exception(int error): error_code(error), message(default_message(error)) {}
            ~driver_exception() throw() {}
            const char* what() const throw() { return message.c_str(); }

            int error_code;
        private:
            std::string message;

        static std::string default_message(int error) {
            switch (error){
                case MAPP_BAD_ARG:
                    return "The arguments are incorrect";
                case MAPP_USAGE:
                    return "Application usage invoked";
                case MAPP_BAD_DATA:
                    return "The input data is incorrect, path?";
                case MAPP_BAD_THREAD:
                    return "Threading problem";
                default:
                    return "Unknown error";
            }
        }
    };

} // end namespace
