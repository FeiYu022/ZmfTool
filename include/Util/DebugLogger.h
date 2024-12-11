#ifndef DEBUG_LOGGER_H
#define DEBUG_LOGGER_H

#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>

enum class LogLevel {
    DEBUGLOG = 0,
    INFOLOG,
    WARNINGLOG,
    ERRORLOG,
    CRITICALLOG,
};

static int defaultLogMaxSize = 1024 * 1024 * 10;
static int defaultLogNum = 5;

class SdkFileLogHandler {
public:
    SdkFileLogHandler(const std::string& fileName)
    {
        logPath_ = "C:\\ProgramData\\Comms\\PCManager\\log\\usr\\" + fileName;
        stream_.open(logPath_, std::ios::out | std::ios::app);
    }

    ~SdkFileLogHandler() {}

    static void SetGlobalFileLogHandler(const std::string& fileName);

    void Log(std::string message, LogLevel)
    {
        std::lock_guard<std::mutex> lck(logMutex_);
        std::string newLogPath;
        stat(logPath_.c_str(), &logStatbuf_);
        if (logStatbuf_.st_size > defaultLogMaxSize) {
            stream_.close();
            for (int i = defaultLogNum; i > 0; i--) {
                std::string tempLogPath = logPath_ + std::to_string(i);
                struct stat tempLogStatbuf;
                if (stat(tempLogPath.c_str(), &tempLogStatbuf) == 0 && i == 5) {
                    remove(tempLogPath.c_str());
                }
                else if (stat(tempLogPath.c_str(), &tempLogStatbuf) == 0) {
                    newLogPath = logPath_ + std::to_string(i + 1);
                    rename(tempLogPath.c_str(), newLogPath.c_str());
                }
            }
            newLogPath = logPath_ + std::to_string(1);
            rename(logPath_.c_str(), newLogPath.c_str());
            stream_.open(logPath_, std::ios::out | std::ios::app);
        }

        std::cout << message;
        stream_ << message;
        stream_.flush();
    }
private:
    std::string logPath_;
    std::fstream stream_;
    std::mutex logMutex_;
    struct stat logStatbuf_;
};

class HnSdkLogger {
public:
    HnSdkLogger(std::string levelName, LogLevel level);
    ~HnSdkLogger();

    void Init(std::string levelName, LogLevel level);
    template<typename T>
    HnSdkLogger& operator<<(T const& value)
    {
        if (logLevel_ >= GetCurrentLogLevel()) {
            stringStream_ << value;
        }
        return *this;
    }

    static LogLevel GetCurrentLogLevel();
    static void SetCurrentLogLevel(LogLevel level);
    void PrintLog();

private:
    static LogLevel currentLogLevel;

    std::ostringstream stringStream_;
    LogLevel logLevel_;

    SdkFileLogHandler* fileLogHandler_ = nullptr;
};


#define LOGGER_ERROR(msg)                                      \
    if (HnSdkLogger::GetCurrentLogLevel() <= LogLevel::ERRORLOG) \
        HnSdkLogger("ERROR", LogLevel::ERRORLOG) << msg << " ("<< __FUNCTION__ << "["<< __LINE__ <<"])";
#define LOGGER_WARN(msg)                                      \
    if (HnSdkLogger::GetCurrentLogLevel() <= LogLevel::WARNINGLOG) \
        HnSdkLogger("WARN", LogLevel::WARNINGLOG) << msg << " ("<< __FUNCTION__ << "["<< __LINE__ <<"])";

#ifdef DEBUG_BUILD
#define LOGGER_INFO(msg)                                      \
    if (HnSdkLogger::GetCurrentLogLevel() <= LogLevel::INFOLOG) \
        HnSdkLogger("INFO", LogLevel::INFOLOG) << msg << " ("<< __FUNCTION__ << "["<< __LINE__ <<"])";
#define LOGGER_DEBUG(msg)                                      \
    if (HnSdkLogger::GetCurrentLogLevel() <= LogLevel::DEBUGLOG) \
        HnSdkLogger("DEBUG", LogLevel::DEBUGLOG) << msg << " ("<< __FUNCTION__ << "["<< __LINE__ <<"])";
#else
#define LOGGER_INFO(msg)   {}
#define LOGGER_DEBUG(msg)   {}
#endif

#endif // DEBUG_LOGGER_H