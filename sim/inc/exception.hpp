#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

#include <iostream>
#include <string>
#include <map>

// Error codes
#define SIM_EXCEPT_ILLEGAL_OP    (1)
#define SIM_EXCEPT_FATAL         (0xfe)
#define SIM_EXCEPT_UNKNOWN       (0xff)

// generic exception class for our sim module
// TODO: This is barebones right now.
class sim_exception : public std::exception {
    // Error code hash
    struct _err_hash : std::map<int, std::string> {
        _err_hash(){
            // All error codes are hashed by code no. into a suitable error message
            (*this)[SIM_EXCEPT_ILLEGAL_OP] = "std::sim_exception : illegal operand";
            (*this)[SIM_EXCEPT_UNKNOWN]    = "std::sim_exception : unknown exception";
            (*this)[SIM_EXCEPT_FATAL]      = "std::sim_exception : fatal error";
        }
        ~_err_hash(){}
    } errhash_;
    int errcode_;
    std::string message_; // Custom user message

    public:
    // Constructor
    sim_exception(int errcode, std::string message = ""){
        if(errhash_.find(errcode) != errhash_.end())
            errcode_ = errcode;
        else
            errcode_ = SIM_EXCEPT_UNKNOWN;

        message_ = message;
    }

    // Destructor
    ~sim_exception() throw(){
    }

    const int err_code(){
        return errcode_;
    }

    // Virtual what for this class
    const char *what() throw(){
        std::string final_message = errhash_[errcode_] + ". " + message_;
        return final_message.c_str();
    }

    // Returns full description
    std::string full_description() throw(){
        return (errhash_[errcode_] + ". " + message_);
    }
};



#endif
