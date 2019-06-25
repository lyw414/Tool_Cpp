#include <string>
#include <stdio.h>
#include <memory>
#include <mutex>
#include <chrono>
#include <time.h>
#include <sstream>
#include <thread>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#include <process.h>
#endif 

#ifdef __linux__
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#endif 

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
    typedef enum _LogLevel
    {
        ERROR_LOG,
        WARNING_LOG,
        DEBUG_LOG
    } TLogLevel;

    typedef enum _LogMode
    {
        TIME_MODE, // echa time for creat a new log file
        SIZE_MODE  // reache size and creat new log file 

    } TLogMode;

    class Simple_Log
    {
    private:
        FILE * m_handle;
        time_t m_Last_Create_New_File_Time;

        std::string m_Log_Tag;
        std::string m_Log_Path;
        std::string m_Log_Name;
        size_t m_Fixed_Time; // seconds
        size_t m_Fixed_Size; // kb

        TLogMode m_Log_Mode;
        TLogLevel m_Log_Level;

        Simple_Log() 
        {
            m_handle = NULL;
            m_Last_Create_New_File_Time = 0;
        };

        std::mutex m_lock; //lock for file operator  
    public:
        //creat dir 
        int mkdir (const std::string & path)
        {
            size_t uiLoop = 0;
            size_t uiLength = 0;
            int ret = 0;
            std::string tmpPath = path;
            char * szPath = NULL;
            if ( tmpPath[tmpPath.length() - 1] != '\\' &&  tmpPath[tmpPath.length() - 1] != '/' )
            {
                tmpPath += '/';
            }

            szPath = (char *)tmpPath.c_str();
            uiLength = tmpPath.length();

            if (szPath[0] == '\\' || szPath[0] == '/')
            {
                uiLoop = 1;
            }
            else
            {
                uiLoop = 0;
            }

            // Creat Dir
            for ( ; uiLoop < uiLength; uiLoop++ )
            {
                if (szPath[uiLoop] == '\\' || szPath[uiLoop] == '/')
                { 
                    szPath[uiLoop] = '\0';
                    //is Dir exist
                    if ( ACCESS (  szPath, 0 ) != 0 )
                    {
                        //Creat Dir
                        if ( ( ret = MKDIR( szPath ) ) != 0 )
                        {
                            return ret;
                        }

                    }
                    szPath[uiLoop] = '/';
                }

            }
            return 0;
        }

        //creat new file
        int CreateNewFile ( )
        {
            //按固定格式生成 日志文件名
            std::string FullName;
            char szLogFileName[128] = {0};
            time_t t;
            tm t_st;
            std::chrono::seconds Now_Time;
            Now_Time = std::chrono::duration_cast < std::chrono::seconds > ( std::chrono::system_clock::now().time_since_epoch());
            t = Now_Time.count();
            LOCALTIME ( &t_st, &t );
            snprintf ( szLogFileName, sizeof(szLogFileName), "%s_%04d%02d%02d_%02d%02d%02d.log",m_Log_Tag.c_str(), t_st.tm_year + 1900, t_st.tm_mon + 1, t_st.tm_mday, t_st.tm_hour,t_st.tm_min,t_st.tm_sec);
            m_Log_Name = std::string ( szLogFileName );
            m_lock.lock();
            //打开文件 若文件不存在则创建 存在则追加写
            m_handle = fopen ( (m_Log_Path + "/" + m_Log_Name).c_str(),"a+");
            m_lock.unlock();
            if ( m_handle == NULL )
            {
                return -1;
            }
            m_Last_Create_New_File_Time = t;
            return 0;
        }

        int CloseFile() 
        {
            m_lock.lock();
            if ( m_handle != NULL )
            {
                fclose(m_handle);
            }
            m_lock.unlock();
            return 0;
        }


        // 0 means not reach create new file condition  
        // 1 means create new file successfully 
        // < 0 means create new file failed
        int Check_And_Create_New_File ( )
        {
        
            switch ( m_Log_Mode )
            {
                case TIME_MODE :
                {
                    time_t now = std::chrono::duration_cast < std::chrono::seconds > ( std::chrono::system_clock::now().time_since_epoch()).count();
                    if ( (size_t)(now - m_Last_Create_New_File_Time) >= m_Fixed_Time )
                    {
                        CreateNewFile();
                        return 1;
                    }
                    break;
                }
                default:
                {
                    struct stat statbuff = { 0 };
                    if ( stat ( (m_Log_Path + "/" + m_Log_Name).c_str(), &statbuff) < 0 )
                    {
                        return -1;
                    }
                    else
                    {
                        if ( (size_t)(statbuff.st_size) >= m_Fixed_Size )
                        {
                            CreateNewFile ();
                            return 1;
                        }
                    }

                    break;
                }

            }
            return 0;
        }
        
    public:
        static std::shared_ptr < Simple_Log > Instance () 
        {
            static std::shared_ptr < Simple_Log >  res  ( new Simple_Log );
            return res;
        }

        int Init(const std::string & Log_Path,const std::string & Log_Tag = "Defaule",const TLogMode Log_Mode = TIME_MODE, const TLogLevel Log_Level = WARNING_LOG, const size_t Fixed_Time = 3600, const size_t Fixed_Size = 1024 )
        {
            int iRet = 0;
            // check Log_Path is usefull make it or make it in default path
            if ( ( iRet = LYW_CODE::Simple_Log::mkdir(Log_Path) ) != 0 )
            {
                m_Log_Path = "./";
            }
            else
            {
                m_Log_Path = Log_Path;
            }
            
            if ( Log_Tag.length() > 64 || Log_Tag.length() == 0 )
            {
                m_Log_Tag = "Dafault";
            }
            else
            {
                m_Log_Tag = Log_Tag;
            }

            //Create Log File
            iRet = CreateNewFile ( );
            if ( iRet != 0 )
            {
                return iRet;
            }
            m_Log_Mode = Log_Mode;
            m_Log_Level = Log_Level;
            m_Fixed_Size = Fixed_Size;
            m_Fixed_Time = Fixed_Time;
            return 0;
        }


        template < typename ... Argv > 
        void Write_Log ( TLogLevel Log_Level,size_t line, const char * filename, const std::string & format, const Argv & ... argv)
        {
            tm t_st;
            time_t t;
            std::string tid;

            std::ostringstream oss;
            if ( m_Log_Level >= Log_Level ) 
            {
                t = std::chrono::duration_cast < std::chrono::milliseconds > ( std::chrono::system_clock::now().time_since_epoch()).count();
                LOCALTIME(&t_st,&t);
                oss << std::this_thread::get_id();
                tid = oss.str();

                m_lock.lock();
                if ( m_handle != NULL )
                {
                    //Set Log Head 
                    switch ( Log_Level )
                    {
                        case ERROR_LOG :
                            fwrite ( "[ERROR]",7,1,m_handle);
                            break;
                        case DEBUG_LOG:
                            fwrite ( "[DEBUG]",7,1,m_handle);
                            break;
                        case WARNING_LOG :
                            fwrite ( "[WARNING]",9,1,m_handle);
                            break;
                        default :
                            fwrite ( "[DEFAULT]",9,1,m_handle);
                    }
                    //Set Log Time PID TID
                    fprintf ( m_handle, "[%04d%02d%02d-%02d%02d%02d %ld][PID:: %d TID:: %s][%s:%ld]::",t_st.tm_year + 1900,t_st.tm_mon + 1,t_st.tm_mday,t_st.tm_hour,t_st.tm_min,t_st.tm_sec,t,(int)GETPID(),tid.c_str(),filename,line );
                    fprintf ( m_handle,format.c_str(), argv...);
                }
                m_lock.unlock();
            }

        }
    };

    static std::shared_ptr < Simple_Log > _g_log_instance = Simple_Log::Instance();

}
#define Init_Log(Log_Path,Log_Tag,Log_Mode,Log_Level,Fixed_Time,Fixed_Size) \
LYW_CODE::_g_log_instance->Init ( Log_Path, Log_Tag,Log_Mode,Log_Level,Fixed_Time,Fixed_Size);
#define UserError(format,args...) LYW_CODE::_g_log_instance->Write_Log ( LYW_CODE::ERROR_LOG,__LINE__,__FILE__,format, ##args );
#define UserWarning(format,args...) LYW_CODE::_g_log_instance->Write_Log ( LYW_CODE::WARNING_LOG,__LIEN__,__FILE__,format, ##args );
#define UserDebug(format,args...) LYW_CODE::_g_log_instance->Write_Log ( LYW_CODE::DEBUG_LOG,__LINE__,__FILE__,format, ##args );
