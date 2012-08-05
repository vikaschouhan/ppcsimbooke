#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

#include <iostream>
#include <string>
#include <map>
// This defines exception codes for machine exceptions
#include "ppc_exception.h"

// Exceptions due to errors ( taken from linux errno.h header )
#define SIM_EXCEPT_EPERM          1    /* Operation not permitted */
#define SIM_EXCEPT_ENOENT         2    /* No such file or directory */
#define SIM_EXCEPT_EIO            5    /* I/O error */
#define SIM_EXCEPT_ENXIO          6    /* No such device or address */
#define SIM_EXCEPT_E2BIG          7    /* Argument list too long */
#define SIM_EXCEPT_ENOEXEC        8    /* Exec format error */
#define SIM_EXCEPT_ENOMEM        12    /* Out of memory */
#define SIM_EXCEPT_EACCES        13    /* Permission denied */
#define SIM_EXCEPT_EFAULT        14    /* Bad address */
#define SIM_EXCEPT_EEXIST        17    /* File exists */
#define SIM_EXCEPT_ENODEV        19    /* No such device */
#define SIM_EXCEPT_ENOTDIR       20    /* Not a directory */
#define SIM_EXCEPT_EISDIR        21    /* Is a directory */
#define SIM_EXCEPT_EINVAL        22    /* Invalid argument */
#define SIM_EXCEPT_ENOSPC        28    /* No space left on device */
#define SIM_EXCEPT_EROFS         30    /* Read-only file system */

#define SIM_EXCEPT_ENOFILE      254    /* File couldn't be opened */
#define SIM_EXCEPT_EUNKWN       255    /* Unknown */

// A global message object used as a temporary placeholder
static std::string glb_msg0;

// generic exception class for our sim module ( only non fatal exception )
// TODO: This is barebones right now.
class sim_exception : public std::exception {
    uint8_t     m_errcode;  // 8 bit error code
    std::string m_message;  // Custom user message

    public:
    // Constructor
    sim_exception(uint64_t errcode, std::string message = "runtime exception"){
        m_errcode    = errcode;
        m_message    = message;
    }
    // Destructor
    ~sim_exception() throw(){
    }

    const uint64_t err_code(){
        return m_errcode;
    }
    const char* desc() const{
        glb_msg0 = m_message;
        return glb_msg0.c_str();
    }
    // Virtual what for this class
    const char *what() const throw(){
        return "runtime simulation exception.";
    }
};

// A fatal exception.
class sim_exception_fatal : public std::exception {
    std::string m_message; // Custom user message

    public:
    // Constructor
    sim_exception_fatal(std::string message = "Fatal error"){
        m_message = message;
    }
    // Destructor
    ~sim_exception_fatal() throw(){
    }
    const char* desc() const{
        glb_msg0 = m_message;
        return glb_msg0.c_str();
    }
    // Virtual what for this class
    const char *what() const throw(){
        return "fatal simulation exception.";
    }
};

// A ppc exception
// All powerPC hardware exceptions are dealt by this class.
// m_errcode[0] -> primary exception number
// m_errcode[1] -> secondary exception number 
//                 It's a bitfield mask signifying the various causes, why 
//                 m_errcode[0] was thrown.
class sim_exception_ppc : public std::exception {
    std::string m_message;     // Custom user message
    uint64_t    m_errcode[2];  // 2 diff codes can be specified for each type

    public:
    // Constructor
    sim_exception_ppc(uint64_t c0, uint64_t c1, std::string message = "PPC exception"){
        m_errcode[0] = c0;
        m_errcode[1] = c1;
        m_message = message;
    }
    // Destructor
    ~sim_exception_ppc() throw(){
    }

    // Retrive Err codes
    const uint64_t err_code0(){
        return m_errcode[0];
    }
    const uint64_t err_code1(){
        return m_errcode[1];
    }
    const char *desc() const{
        glb_msg0 = m_message;
        return glb_msg0.c_str();
    } 
    // Virtual what for this class
    const char *what() const throw(){
        return "ppc exception.";
    }
};

#endif
