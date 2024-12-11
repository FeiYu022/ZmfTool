#include "Util/DebugLogger.h"

#include <iomanip>

using namespace std;

static SdkFileLogHandler g_logHandler("IPCNamePipe.log");
LogLevel HnSdkLogger::currentLogLevel = LogLevel::DEBUGLOG;


void SdkFileLogHandler::SetGlobalFileLogHandler(const std::string& fileName)
{
    g_logHandler.stream_.close();
    g_logHandler.logPath_ = "C:\\ProgramData\\Comms\\PCManager\\log\\usr\\" + fileName;
    g_logHandler.stream_.open(g_logHandler.logPath_, std::ios::out | std::ios::app);
}

HnSdkLogger::HnSdkLogger(std::string levelName, LogLevel level)
{
    Init(levelName, level);
}

HnSdkLogger::~HnSdkLogger()
{
    stringStream_ << std::endl;
    if (logLevel_ >= GetCurrentLogLevel()) {
        PrintLog();
        if (fileLogHandler_ != nullptr) {
            fileLogHandler_->Log(stringStream_.str(), logLevel_);
        }
    }
}

void HnSdkLogger::Init(std::string levelName, LogLevel level)
{
    DWORD currentProcessId = GetCurrentProcessId();
    DWORD currentThreadId = GetCurrentThreadId();
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    stringStream_ << sysTime.wYear << "-";
    stringStream_ << setfill('0') << setw(2) << sysTime.wMonth << "-";
    stringStream_ << setfill('0') << setw(2) << sysTime.wDay << " ";
    stringStream_ << setfill('0') << setw(2) << sysTime.wHour << ":";
    stringStream_ << setfill('0') << setw(2) << sysTime.wMinute << ":";
    stringStream_ << setfill('0') << setw(2) << sysTime.wSecond << ",";
    stringStream_ << setfill('0') << setw(3) << sysTime.wMilliseconds;
    stringStream_ << "[" << currentProcessId << "]";
    stringStream_ << "[" << currentThreadId << "] ";
    stringStream_ << "[" << levelName << "] - ";
    stringStream_ << "[HnOfficeSdk] ";

    logLevel_ = level;
    fileLogHandler_ = &g_logHandler;
}

LogLevel HnSdkLogger::GetCurrentLogLevel()
{
    return HnSdkLogger::currentLogLevel;
}

void HnSdkLogger::SetCurrentLogLevel(LogLevel level)
{
    HnSdkLogger::currentLogLevel = level;
}

void HnSdkLogger::PrintLog()
{
    std::cout << stringStream_.str();
}
