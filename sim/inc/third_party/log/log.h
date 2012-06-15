#ifndef __Log_H__
#define __Log_H__

#include <sstream>
#include <string>
#include <cstdio>
#include <sys/time.h>

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

enum Loglevel {logERROR, logWARNING, logINFO, logDEBUG, logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4};

// Log class
class Log
{
    public:
    Log(){
    }
    // Destructor flushes the stream automatically
    ~Log(){
        os << std::endl;
        output(os.str());
    }

    // Get the output stream object
    std::ostringstream& get(Loglevel level = logINFO){
        os << "- " << now_time();
        os << " " << to_string(level) << ": ";
        os << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');
        return os;
    }

    // Reporting level
    static Loglevel& reporting_level(){
        static Loglevel reportinglevel = logDEBUG4;
        return reportinglevel;
    }

    // convert Log level to string reperesentation
    static std::string to_string(Loglevel level){
        static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4"};
        return buffer[level];
    }

    // Convert from string
    static Loglevel from_string(const std::string& level){
        if (level == "DEBUG4")
            return logDEBUG4;
        if (level == "DEBUG3")
            return logDEBUG3;
        if (level == "DEBUG2")
            return logDEBUG2;
        if (level == "DEBUG1")
            return logDEBUG1;
        if (level == "DEBUG")
            return logDEBUG;
        if (level == "INFO")
            return logINFO;
        if (level == "WARNING")
            return logWARNING;
        if (level == "ERROR")
            return logERROR;
        Log().get(logWARNING) << "Unknown Logging level '" << level << "'. Using INFO level as default.";
        return logINFO;
    }

    // Start diverting to this file
    static void direct_to_file(std::string file_name){
        // Don't close stream if STDOUT or STDERR
        if((stream != NULL) && (stream != stdout) && (stream != stderr)){
            fclose(stream);
        }
        stream = fopen(file_name.c_str(), "w");
    }

    private:
    inline void output(const std::string& msg)
    {   
        if (!stream)
            return;
        fprintf(stream, "%s", msg.c_str());
        fflush(stream);
    }

    protected:
    std::ostringstream os;
    static FILE* stream;

    private:
    Log(const Log&);
    Log& operator =(const Log&);
};

FILE* Log::stream = stdout;

#ifndef FILELog_MAX_LEVEL
#define FILELog_MAX_LEVEL logDEBUG4
#endif

#define Log___(level) \
    if ((level) > FILELog_MAX_LEVEL) ;\
    else if ((level) > Log::reporting_level()) ; \
    else Log().get(level)

#define LOG(level_str) Log___(Log::from_string(level_str))
#define LOG_TO_FILE(file_name) Log::direct_to_file(file_name)

// for Boost.Python
// We don't want to expose the whole logging framewok, only the log_to_file functionality
inline void log_to_file(std::string file_name){
    LOG_TO_FILE(file_name);
}

#endif //__Log_H__
