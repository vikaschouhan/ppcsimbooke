#ifndef _LOG_HPP
#define _LOG_HPP

//  log.hpp 
//  This file contains logging facilities for ppcsim.
//
//  Copyright 2012.
//  Author : Vikas Chouhan ( presentisgood@gmail.com )
//
//  This file is part of ppc-sim.
//
//  This library is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3, or (at your option)
//  any later version.
//
//  It is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//  License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this file; see the file COPYING.  If not, write to the
//  Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
//  MA 02110-1301, USA.

#include <string>
#include <fstream>
#include <map>
#include <iostream>
#include <sys/time.h>

template<int EN_LOG> class Log
{
    std::ostream      *m_stream;
    std::ofstream     m_fstream;
    bool              m_enable_log;
    int               m_log_level;
#define LOG_LEVEL_DEBUG       (0x1)
#define LOG_LEVEL_DEBUG1      (0x2)
#define LOG_LEVEL_DEBUG2      (0x3)
#define LOG_LEVEL_DEBUG3      (0x4)
#define LOG_LEVEL_DEBUG4      (0x5)
#define LOG_LEVEL_ERROR       (0x6)
#define LOG_LEVEL_WARNING     (0x7)
#define LOG_LEVEL_INFO        (0x8)

    private:
    inline std::string now_time()
    {
        char buffer[11];
        time_t t;
        time(&t);
        tm r = {0};
        strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
        struct timeval tv;
        gettimeofday(&tv, 0);
        char result[100] = {0};
        std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000); 
        return result;
    }

    inline std::string to_str(int level){
        static std::map<int, std::string>  lev2str = {
            std::make_pair(LOG_LEVEL_DEBUG,   "DEBUG"  ),
            std::make_pair(LOG_LEVEL_DEBUG1,  "DEBUG1" ),
            std::make_pair(LOG_LEVEL_DEBUG2,  "DEBUG2" ),
            std::make_pair(LOG_LEVEL_DEBUG3,  "DEBUG3" ),
            std::make_pair(LOG_LEVEL_DEBUG4,  "DEBUG4" ),
            std::make_pair(LOG_LEVEL_ERROR,   "ERROR"  ),
            std::make_pair(LOG_LEVEL_WARNING, "WARNING"),
            std::make_pair(LOG_LEVEL_INFO,    "INFO"   )
        };
        return lev2str[level];
    }

    inline int from_str(std::string level){
        static std::map<std::string, int>  str2lev = {
            std::make_pair("DEBUG"    ,LOG_LEVEL_DEBUG   ),
            std::make_pair("DEBUG1"   ,LOG_LEVEL_DEBUG1  ),
            std::make_pair("DEBUG2"   ,LOG_LEVEL_DEBUG2  ),
            std::make_pair("DEBUG3"   ,LOG_LEVEL_DEBUG3  ),
            std::make_pair("DEBUG4"   ,LOG_LEVEL_DEBUG4  ),
            std::make_pair("ERROR"    ,LOG_LEVEL_ERROR   ),
            std::make_pair("WARNING"  ,LOG_LEVEL_WARNING ),
            std::make_pair("INFO"     ,LOG_LEVEL_INFO    )
        };
        return str2lev[level];
    }

    public:
    Log(){
        m_stream = &(std::cout);
        m_enable_log = true;
        m_log_level = LOG_LEVEL_INFO;
    }
    Log(std::string filename){
        m_fstream.open(filename.c_str());
        m_stream = &m_fstream;
        m_enable_log = true;
        m_log_level = LOG_LEVEL_INFO;
    }
    ~Log(){
        m_stream = NULL;
        m_fstream.close();
    }

    void direct_to_file(std::string filename){
        m_fstream.close();
        m_fstream.open(filename.c_str());
        m_stream = &m_fstream;
    }

    void enable(){
        m_enable_log = true;
    }

    void disable(){
        m_enable_log = false;
    }

    // Overload << for normal buildin types
    template<typename T> Log& operator<<(T item)
    {
        if(EN_LOG){
            if(m_enable_log == true){
                (*m_stream) << item;
            }
        }
        return *this;
    }

    // overload for stream manipulators ( std::endl etc for eg. )
    Log& operator<<(std::ostream& (*pf)(std::ostream&)){
        if(EN_LOG){
            if(m_enable_log == true){
                (*m_stream) << pf;
            }
        }
        return *this;
    }

    Log& operator()(std::string level){
        if(EN_LOG){
            if(m_enable_log == true){
                (*m_stream) << "- " << now_time() << " [" << level << "]" << '\t';
            }
        }
        return *this;
    }

    Log& operator()(int level){
        return (*this)(to_str(level));
    }
};

#ifndef SIM_DEBUG
#define SIM_DEBUG 1
#endif

static Log<SIM_DEBUG> Logger;

#define LOG(level_str) Logger(level_str)
#define LOG_TO_FILE(file_name) Logger.direct_to_file(file_name)

// for Boost.Python
// We don't want to expose the whole logging framewok, only the log_to_file functionality
inline void log_to_file(std::string file_name){
    LOG_TO_FILE(file_name);
}

#endif
