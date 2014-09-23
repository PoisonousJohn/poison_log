#pragma once

#define TRACE(...)  poison::utils::log(poison::utils::LogLevel::LOG_TRACE,   __FILE__, __LINE__, __VA_ARGS__)
#define DBG(...)    poison::utils::log(poison::utils::LogLevel::LOG_DEBUG,   __FILE__, __LINE__, __VA_ARGS__)
#define WARN(...)   poison::utils::log(poison::utils::LogLevel::LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define ERR(...)    poison::utils::log(poison::utils::LogLevel::LOG_ERROR,   __FILE__, __LINE__, __VA_ARGS__)

#ifndef POISON_LOG_SYNCHRONIZED
    #define POISON_LOG_SYNCHRONIZED 0
#endif

#include <sstream>
#include <boost/format.hpp>
#include <iomanip> // put_time
#include <chrono>
#include <string>

#if POISON_LOG_SYNCHRONIZED
    #ifdef __ANDROID__
        #include <boost/thread/mutex.hpp>
        #include <boost/thread/lock_guard.hpp>
    #else
        #include <mutex>
    #endif
#endif

#ifdef __ANDROID__
    #include <android/log.h>
#endif

namespace poison { namespace utils {

#if POISON_LOG_SYNCHRONIZED
    #ifdef __ANDROID__
        typedef boost::mutex Mutex;
        typedef boost::lock_guard<Mutex> Lock;
    #else
        typedef std::mutex Mutex;
        typedef std::lock_guard<Mutex> Lock;
    #endif

    static Mutex logMutex;
#endif

    enum class LogLevel {
        LOG_NULL = 0, 
        LOG_ERROR, 
        LOG_WARNING, 
        LOG_DEBUG, 
        LOG_TRACE
    };
    
    namespace internal{
        static const char * appTag = "app";
        static LogLevel logLevel = LogLevel::LOG_DEBUG;
    };

    static std::string currentDateTime() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        struct tm time;
        localtime_r(&in_time_t, &time);
        
        std::stringstream ss;
        //    ss << std::ctime(&in_time_t);
        ss << std::put_time(&time, "%Y-%m-%d %X");
        return ss.str();
    }

    static void setAppTag(const char * t){
        internal::appTag = t;
    }
    
    static void setAppTag(const std::string& t){
        internal::appTag = t.c_str();
    }
    
    static void setLogLevel(LogLevel l) {
        internal::logLevel = l;
    }
    
    static std::string getAppTag(){
        return std::string(internal::appTag);
    }
    
    static LogLevel getLogLevel(){
        return internal::logLevel;
    }

    static std::string formatString(boost::format& message) {
        return message.str();
    }
    
    template<typename TValue, typename... TArgs>
    static std::string formatString(boost::format& message, TValue arg, TArgs... args) {
        message % arg;
        return formatString(message, args...);
    }
    
    template<typename... TArgs>
    static std::string formatString(const std::string& fmt, TArgs... args) {
        boost::format message(fmt);
        return formatString(message, args...);
    }
    
    template<typename ...Args>
    static void log(LogLevel priority, const char * fileName, int lineNumber, const char * format, Args ... args) {
#if POISON_LOG_SYNCHRONIZED
        Lock lock(logMutex);
#endif

        if (priority > internal::logLevel) {
            return;
        }
        
        static const char *LogLevel_str[] = {"", "E", "W", "D", "T"};

        std::ostringstream stream;
        stream  << currentDateTime().c_str() << " [ " << LogLevel_str[int(priority)] << "] " << formatString(format, args...) << std::endl;
        
#ifdef __APPLE__
        if(appTag != ""){
            std::cout << appTag << ": " << stream.str();
        }else{
            std::cout << stream.str();
        }
#elif __ANDROID__

        android_LogPriority aPriority = android_LogPriority::ANDROID_LOG_UNKNOWN;
        switch(priority){
            case LogLevel::LOG_TRACE:
                aPriority = android_LogPriority::ANDROID_LOG_INFO;
            break;

            case LogLevel::LOG_DEBUG:
                aPriority = android_LogPriority::ANDROID_LOG_DEBUG;
            break;

            case LogLevel::LOG_WARNING:
                aPriority = android_LogPriority::ANDROID_LOG_WARN;
            break;

            case LogLevel::LOG_ERROR:
                aPriority = android_LogPriority::ANDROID_LOG_ERROR;
            break;

            default:
                aPriority = android_LogPriority::ANDROID_LOG_UNKNOWN;
            break;
        }

        __android_log_write(ANDROID_LOG_INFO, internal::appTag, stream.str().c_str() );

#endif
        
    }

} }