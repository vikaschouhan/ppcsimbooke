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

#define SIM_EXCEPT_ENOFILE       50    /* File couldn't be opened */
#define SIM_EXCEPT_EUNKWN        51    /* Unknown */
#define SIM_EXCEPT_ELIMIT        52    // Crossing limit

// Exceptions due to some other factors
#define SIM_EXCEPT_SBKPT        100    // Software breakpoint

// A global message object used as a temporary placeholder
static std::string glb_msg0;

// generic exception class for our sim module ( only non fatal exception )
// TODO: This is barebones right now.
class sim_except : public std::exception {
    int         m_errcode;  // 8 bit error code
    std::string m_message;  // Custom user message

    public:
    // Constructor
    sim_except(int errcode, std::string message = "runtime exception"){
        m_errcode    = errcode;
        m_message    = message;
    }
    // Destructor
    ~sim_except() throw(){
    }

    const int err_code(){
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
class sim_except_fatal : public std::exception {
    std::string m_message; // Custom user message

    public:
    // Constructor
    sim_except_fatal(std::string message = "Fatal error"){
        m_message = message;
    }
    // Destructor
    ~sim_except_fatal() throw(){
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
class sim_except_ppc : public std::exception {
    std::string m_message;     // Custom user message
    int         m_errcode[2];  // 2 diff codes can be specified for each type
    uint64_t    m_addr;        // Possible offending address

    public:
    // Constructor
    sim_except_ppc(int c0, int c1, std::string message = "PPC exception", uint64_t a=0){
        m_errcode[0] = c0;
        m_errcode[1] = c1;
        m_message    = message;
        m_addr       = a;
    }
    sim_except_ppc(int c0, int c1, uint64_t a=0){
        m_errcode[0] = c0;
        m_errcode[1] = c1;
        m_addr       = a;
    }
    sim_except_ppc(int c0, std::string message = "PPC exception"){
        m_errcode[0] = c0;
        m_message    = message;
    }

    // Destructor
    ~sim_except_ppc() throw(){
    }

    // Retrive Err codes
    const int err_code0(){
        return m_errcode[0];
    }
    const int err_code1(){
        return m_errcode[1];
    }
    template<int x>
    const int err_code(){
        return m_errcode[x];
    }
    const int addr(){
        return m_addr;
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

// A special exception which halt the processor.
// This is separated out to distinguish it from normal ppc exception ( sim_except_ppc )
// which might be required to be caught
class sim_except_ppc_halt : public std::exception {
    std::string m_message;     // Custom user message

    public:
    // Constructor
    sim_except_ppc_halt(std::string message = "Machine halted."){
        m_message = message;
    }
    // Destructor
    ~sim_except_ppc_halt() throw(){
    }

    const char *desc() const{
        glb_msg0 = m_message;
        return glb_msg0.c_str();
    } 
    // Virtual what for this class
    const char *what() const throw(){
        return "ppc halt exception.";
    }
};

// global exception_ptr object
static std::exception_ptr sim_except_ptr;

#endif
