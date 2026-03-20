#include "logger.h"

#include <sstream>      // 字符串流
#include <chrono>       // 时间戳
#include <iomanip>      // 格式化时间戳
#include <iostream>

#ifdef _WIN32 // Windows/MSVC环境
#define LOCALTIME_R(time_t_ptr, tm_ptr) localtime_s(tm_ptr, time_t_ptr)
#else // Linux/GCC环境
#define LOCALTIME_R(time_t_ptr, tm_ptr) localtime_r(time_t_ptr, tm_ptr)
#endif


/********************抽象基类日志器********************/
Logger::~Logger() = default;

std::string Logger::levelToString(const Logger::LogLevel & level) const
{
    switch (level) {
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARN: return "WARN";
    case LogLevel::ERROR: return "ERROR";
    default: return "UNKNOWN";
    }
}

void Logger::log(const std::string & msg, const Logger::LogLevel & level, const std::string & module)
{
    if (m_filter.load() & static_cast<unsigned char>(level)) {
        return; // 过滤
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    try {
        logOut(msg, level, module);
    }
    catch (const LogException& e) {
        // 日志类型异常
        std::cerr << "[LOG_EXCEPTION] [" << module << "] [" << levelToString(level) << "] "
            << "Log out failed: " << e.what() << ", log: " << msg << std::endl;
    }
    catch (const std::ios_base::failure& e) {
        // 标准IO/文件异常
        std::cerr << "[LOG_IO_ERROR] [" << module << "] [" << levelToString(level) << "] "
            << "File operation failed: " << e.what() << ", log: " << msg << std::endl;
    }
    catch (const std::exception& e) {
        // 其他标准异常
        std::cerr << "[LOG_UNKNOWN_ERROR] [" << module << "] [" << levelToString(level) << "] "
            << "Unknown error: " << e.what() << ", log: " << msg << std::endl;
    }
    catch (...) {
        // 未知致命异常
        std::cerr << "[LOG_FATAL_ERROR] [" << module << "] [" << levelToString(level) << "] "
            << "Fatal unknown error, log:" << msg << std::endl;
    }
}

Logger& Logger::operator <<(const std::string & msg)
{
    this->log(msg, LogLevel::DEBUG);
    return *this;
}


// 工具函数：生成格式化时间戳（格式：YYYY-MM-DD HH:MM:SS.ms）
std::string Logger::getTimesTamp()
{
    auto now = std::chrono::system_clock::now();    // 获取当前时间
    auto in_time_t = std::chrono::system_clock::to_time_t(now); // 转换为time_t
    // 提取到毫秒
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tm_buf{}; // 本地缓冲区，避免静态全局缓冲区竞态
    if (LOCALTIME_R(&in_time_t, &tm_buf) != 0) {
        // 转换失败时的兜底（如时间值非法）
        return "2000-01-01 00:00:00.000";
    }
    // 格式化时间
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}
