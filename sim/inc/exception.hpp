#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

#include <iostream>
#include <string>
#include <map>
// This defines exception codes for machine exceptions
#include "ppc_exception.h"

// Error codes
#define SIM_EXCEPT_ILLEGAL_OP          ( 0x1)
#define SIM_EXCEPT_RESOURCE_UNAVAIL    ( 0x2)
#define SIM_EXCEPT_PPC                 (0x20)
#define SIM_EXCEPT_UNKNOWN             (0xff)

// generic exception class for our sim module ( only non fatal exception )
// PPC exceptions are also dealt by our simulator exception mechanism.
// TODO: This is barebones right now.
class sim_exception : public std::exception {
    // Error code hash
    struct _err_hash : std::map<int, std::string> {
        _err_hash(){
            // All error codes are hashed by code no. into a suitable error message
            (*this)[SIM_EXCEPT_ILLEGAL_OP]       = "std::sim_exception : illegal operand";
            (*this)[SIM_EXCEPT_UNKNOWN]          = "std::sim_exception : unknown exception";
            (*this)[SIM_EXCEPT_RESOURCE_UNAVAIL] = "std::sim_exception : Resource unavailable";
            (*this)[SIM_EXCEPT_PPC]              = "std::sim_exception : Powerpc exception";
        }
        ~_err_hash(){}
    } m_errhash;
    uint64_t m_errcode[4];  // maximum 4 error code heirarchies
                            // in case of PPC exceptions, m_errcode[0] = SIM_EXCEPT_PPC
                            //                            m_errcode[1] = PPC_EXCEPTION_xxx
                            //                            m_errcode[2] = PPC_EXCEPT_xxx_yyy
    std::string m_message;  // Custom user message

    public:
    // Constructor
    sim_exception(uint64_t errcode, std::string message = ""){
        if(m_errhash.find(errcode) != m_errhash.end())
            m_errcode[0] = errcode;
        else
            m_errcode[0] = SIM_EXCEPT_UNKNOWN;

        m_message    = message;
        m_errcode[1] = -1;
        m_errcode[2] = -1;
        m_errcode[3] = -1;
    }
    sim_exception(uint64_t errcode, uint64_t code0){
        if(m_errhash.find(errcode) != m_errhash.end())
            m_errcode[0] = errcode;
        else
            m_errcode[0] = SIM_EXCEPT_UNKNOWN;

        m_message     = "";
        m_errcode[1]  = code0;
        m_errcode[2]  = -1;
        m_errcode[3]  = -1;
    }
    sim_exception(uint64_t errcode, uint64_t code0, uint64_t code1){
        if(m_errhash.find(errcode) != m_errhash.end())
            m_errcode[0] = errcode;
        else
            m_errcode[0] = SIM_EXCEPT_UNKNOWN;

        m_message     = "";
        m_errcode[1]  = code0;
        m_errcode[2]  = code1;
        m_errcode[3]  = -1;
    }

    // Destructor
    ~sim_exception() throw(){
    }

    const uint64_t err_code(){
        return m_errcode[0];
    }

    const uint64_t sec_err_code(int num){
        return m_errcode[1+num];
    }

    // Virtual what for this class
    const char *what() throw(){
        std::string final_message = m_errhash[m_errcode[0]] + ". " + m_message;
        return final_message.c_str();
    }

    // Returns full description
    std::string full_description() throw(){
        return (m_errhash[m_errcode[0]] + ". " + m_message);
    }
};

// A fatal exception.
class sim_exception_fatal : public std::exception {
   std::string m_message; // Custom user message

    public:
    // Constructor
    sim_exception_fatal(std::string message = ""){
        m_message = message;
    }
    // Destructor
    ~sim_exception_fatal() throw(){
    }
    // Virtual what for this class
    const char *what() throw(){
        return m_message.c_str();
    }
};

#endif
