#include "utils/Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

// Static member initialization
Logger* Logger::s_instance = nullptr;

Logger& Logger::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new Logger();
    }
    return *s_instance;
}

void Logger::DestroyInstance()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

void Logger::SetFileLogging(bool enable, const std::string& filename)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_fileLogging = enable;
    if (m_fileLogging)
    {
        if (m_logFile.is_open())
        {
            m_logFile.close();
        }
        m_logFile.open(filename, std::ios::app);
    }
    else
    {
        if (m_logFile.is_open())
        {
            m_logFile.close();
        }
    }
}

void Logger::SetConsoleLogging(bool enable)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_consoleLogging = enable;
}

void Logger::SetLogLevel(int level)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_level = level;
}

void Logger::Log(int level, const std::string& message)
{
    if (level < m_level)
        return;

    WriteLog(level, message);
}

void Logger::Debug(const std::string& message)
{
    Log(0, message);
}

void Logger::Info(const std::string& message)
{
    Log(1, message);
}

void Logger::Warning(const std::string& message)
{
    Log(2, message);
}

void Logger::Error(const std::string& message)
{
    Log(3, message);
}

void Logger::Critical(const std::string& message)
{
    Log(4, message);
}

std::string Logger::GetLevelString(int level)
{
    switch (level)
    {
        case 0: return "DEBUG";
        case 1: return "INFO";
        case 2: return "WARNING";
        case 3: return "ERROR";
        case 4: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string Logger::GetCurrentTimeString()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    struct tm timeinfo;
    localtime_s(&timeinfo, &time_t);
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

void Logger::WriteLog(int level, const std::string& message)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string timestamp = GetCurrentTimeString();
    std::string levelStr = GetLevelString(level);
    std::string logMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    if (m_consoleLogging)
    {
        std::cout << logMessage << std::endl;
    }
    
    if (m_fileLogging && m_logFile.is_open())
    {
        m_logFile << logMessage << std::endl;
        m_logFile.flush();
    }
}