#pragma once
#include <mutex>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <chrono>
#include <string>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <sstream>
#include <stdarg.h>
#include <thread>

namespace LYW_CODE
{
    #ifdef WIN32
    #define ACCESS ::_access
    #define MKDIR(dir) ::_mkdir((dir))
    #define LOCALTIME(t_st,t) std::localtime_s(t_st,t)
    #define GETPID _getpid
    #endif
    
    #ifdef __linux__
    #define ACCESS ::access
    #define MKDIR(dir) ::mkdir((dir),0755)
    #define LOCALTIME(t_st,t) ::localtime_r(t,t_st)
    #define GETPID getpid
    #endif

    class log
    {
    public:
        typedef enum _logLevel
        {
            NoLog,
            Error,
            Warning,
            Debug
        }TlogLevel;

    static std::mutex m_lock;

    private:
        /*print to stdout :: 0 no 1 yes*/
        static int printFlg;
        /*Log Level use log::TLogLevel*/
        static _logLevel logLevel;
        static int logMode;
        static int timeInterval;
        static int maxFileSize;
        static std::string logHead;
        static std::string logPath;
        /*static std::string logformat;*/
        /*used for pthread not for thread*/
        static FILE * fHandle;

        static std::time_t now;
        static std::time_t last;

    public:

        static int CreateDir()
        {
            size_t uiLoop = 0;
            size_t uiLength = 0;
            int ret = 0;
            std::string tmpPath = log::logPath;
            char* szPath = NULL;
            if (tmpPath[tmpPath.length() - 1] != '\\' && tmpPath[tmpPath.length() - 1] != '/')
            {
                tmpPath += '/';
            }

            szPath = (char*)tmpPath.c_str();
            uiLength = tmpPath.length();

            if (szPath[0] == '\\' || szPath[0] == '/')
            {
                uiLoop = 1;
            }
            else
            {
                uiLoop = 0;
            }

            /* Creat Dir*/
            for (; uiLoop < uiLength; uiLoop++)
            {
                if (szPath[uiLoop] == '\\' || szPath[uiLoop] == '/')
                {
                    szPath[uiLoop] = '\0';
                    /*is Dir exist*/
                    if (ACCESS(szPath, 0) != 0)
                    {
                        /*Creat Dir*/
                        if ((ret = MKDIR(szPath)) != 0)
                        {
                            return ret;
                        }
                    }
                    szPath[uiLoop] = '/';
                }
            }
            return 0;
        }

        static int CreateNewFile()
        {
            tm t = { 0 };
            int ret = 0;
            std::string fileName;
            /*create new file check*/
            switch (log::logMode)
            {
            case 0:
                /*time check*/
                if (log::now - log::last >= log::timeInterval || log::last == 0)
                {
                    if (log::fHandle != NULL )
                    {
                        ::fclose(log::fHandle);
                    }
                    log::last = log::now;
                }
                else if ( log::now - log::last < 0 )
                {
                    /*time change reset it*/
                    log::last = log::now;
                    return 1;
                }
                else
                {
                    return 1;
                }
                localtime_r(&log::now, &t);
                fileName = log::logPath + "/" + log::logHead + '_' + std::to_string(t.tm_year + 1900) + "-" + std::to_string(t.tm_mon + 1 ) + "-" + std::to_string(t.tm_mday) + "-" +std::to_string(t.tm_hour ) + "-"+ std::to_string(t.tm_min ) + "-" + std::to_string(t.tm_sec) + ".log";
                break;
            case 1:
                /*size check*/
                return 1;
            case 2:
                if (log::last == 0)
                {
                    fileName = log::logPath + "/" + log::logHead + ".log";
                    break;
                }
                else
                {
                    return 1;
                }
            }
            /*create directory*/
            if ( ( ret = CreateDir() ) < 0 )
            {
                return ret;
            }
            log::fHandle = ::fopen(fileName.c_str(),"a+");
            if (log::fHandle == NULL)
            {
                return -2;
            }
            return 1;
        }

    public:
        template < typename ... Argv >
        static void Write_Log(log::TlogLevel Log_Level, size_t line, const char * filename,  const std::string & format, const Argv & ... argv )
        {
            struct tm tv;
            std::ostringstream oss;
            log::m_lock.lock();
            log::now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            log::CreateNewFile();
            time_t t = std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch()).count();
            time_t t1 = std::chrono::duration_cast <std::chrono::seconds> (std::chrono::system_clock::now().time_since_epoch()).count();
            LOCALTIME(&tv, &t1);
            oss << std::this_thread::get_id();
            std::string tid = oss.str();
            switch (Log_Level)
            {
            case log::NoLog:
                break;
            case log::Error:
                ::fwrite("[ERROR]", 7, 1, log::fHandle);
                ::fprintf(log::fHandle, "[%04d%02d%02d-%02d%02d%02d %ld][PID:: %d TID:: %s][%s:%ld]::", tv.tm_year + 1900, tv.tm_mon + 1, tv.tm_mday, tv.tm_hour, tv.tm_min, tv.tm_sec, t, (int)GETPID(), tid.c_str(), filename, line);
                ::fprintf(log::fHandle, format.c_str(), argv...);
                ::fwrite("\n", 1, 1, log::fHandle);
                break;
            case log::Warning:
                ::fwrite("[WARMING]",9, 1, log::fHandle);
                ::fprintf(log::fHandle, "[%04d%02d%02d-%02d%02d%02d %ld][PID:: %d TID:: %s][%s:%ld]::", tv.tm_year + 1900, tv.tm_mon + 1, tv.tm_mday, tv.tm_hour, tv.tm_min, tv.tm_sec, t, (int)GETPID(), tid.c_str(), filename, line);
                ::fprintf(log::fHandle, format.c_str(), argv...);
                ::fwrite("\n", 1, 1, log::fHandle);
                break;
            case log::Debug:
                ::fwrite("[DEBUG]", 7, 1, log::fHandle);
                ::fprintf(log::fHandle, "[%04d%02d%02d-%02d%02d%02d %ld][PID:: %d TID:: %s][%s:%ld]::", tv.tm_year + 1900, tv.tm_mon + 1, tv.tm_mday, tv.tm_hour, tv.tm_min, tv.tm_sec, t, (int)GETPID(), tid.c_str(), filename, line);
                ::fprintf(log::fHandle, format.c_str(), argv...);
                ::fwrite("\n", 1, 1, log::fHandle);
                break;
            default:
                ::fwrite("[DEFAULT]", 7, 1, log::fHandle);
                ::fprintf(log::fHandle, "[%04d%02d%02d-%02d%02d%02d %ld][PID:: %d TID:: %s][%s:%ld]::", tv.tm_year + 1900, tv.tm_mon + 1, tv.tm_mday, tv.tm_hour, tv.tm_min, tv.tm_sec, t, (int)GETPID(), tid.c_str(), filename, line);
                ::fprintf(log::fHandle, format.c_str(), argv...);
                ::fwrite("\n", 1, 1, log::fHandle);
                break;
            }

            if (log::printFlg == 1)
            {
                switch (Log_Level)
                {
                case log::Error:
                    ::printf(("[ERROR][%04d%02d%02d-%02d%02d%02d %ld][PID:: %d TID:: %s][%s:%ld]::" + format + "\n").c_str(), tv.tm_year + 1900, tv.tm_mon + 1, tv.tm_mday, tv.tm_hour, tv.tm_min, tv.tm_sec, t, (int)GETPID(), tid.c_str(), filename, line, argv...);
                    break;
                case log::Warning:
                    ::printf(("[WARNING][%04d%02d%02d-%02d%02d%02d %ld][PID:: %d TID:: %s][%s:%ld]::" + format + "\n").c_str(), tv.tm_year + 1900, tv.tm_mon + 1, tv.tm_mday, tv.tm_hour, tv.tm_min, tv.tm_sec, t, (int)GETPID(), tid.c_str(), filename, line, argv...);
                    break;
                case log::Debug:
                    ::printf(("[DEBUG][%04d%02d%02d-%02d%02d%02d %ld][PID:: %d TID:: %s][%s:%ld]::" + format + "\n").c_str(), tv.tm_year + 1900, tv.tm_mon + 1, tv.tm_mday, tv.tm_hour, tv.tm_min, tv.tm_sec, t, (int)GETPID(), tid.c_str(), filename, line, argv...);
                    break;
                default:
                    ::printf(("[DEFAULT][%04d%02d%02d-%02d%02d%02d %ld][PID:: %d TID:: %s][%s:%ld]::" + format + "\n").c_str(), tv.tm_year + 1900, tv.tm_mon + 1, tv.tm_mday, tv.tm_hour, tv.tm_min, tv.tm_sec, t, (int)GETPID(), tid.c_str(), filename, line, argv...);
                }
            }
            log::m_lock.unlock();
        }

        static void setPrintFlg( int printFlg) 
        {
            m_lock.lock();
            log::printFlg = printFlg;
            m_lock.unlock();
        }

        static void setLogLevel(log::TlogLevel level) 
        {
            m_lock.lock();
            logLevel = level;
            m_lock.unlock();
        }

        static void setLogMode(int mode)
        {
            m_lock.lock();
            logMode= mode;
            m_lock.unlock();
        }

        static void setTimeInterval(int interval)
        {
            m_lock.lock();
            timeInterval = interval;
            m_lock.unlock();
        }

        static void setMaxFileSize(int fileSize)
        {
            m_lock.lock();
            maxFileSize = fileSize;
            m_lock.unlock();
        }

        static void setfileHead( std::string fileHead )
        {
            m_lock.lock();
            logHead = fileHead;
            m_lock.unlock();
        }

        static void setPath( std::string path )
        {
            m_lock.lock();
            logPath = path;
            log::last = 0;
            CreateNewFile();
            m_lock.unlock();
        }
    };

    /*param init*/
    int log::printFlg = 0;
    log::_logLevel log::logLevel = log::Warning;
    int log::logMode = 0;
    int log::timeInterval = 3600;
    int log::maxFileSize = 10240;
    std::string log::logHead = "Default";
    std::string log::logPath = "./log/";
    std::mutex log::m_lock;

    std::time_t log::last = 0;
    std::time_t log::now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    FILE * log::fHandle = NULL;

    #define UserError(format,args...) LYW_CODE::log::Write_Log( LYW_CODE::log::Error,__LINE__,__FILE__,format, ##args );
    #define UserWarning(format,args...) LYW_CODE::log::Write_Log ( LYW_CODE::log::Warning,__LIEN__,__FILE__,format, ##args );
    #define UserDebug(format,args...) LYW_CODE::log::Write_Log ( LYW_CODE::log::Debug,__LINE__,__FILE__,format, ##args );
}

