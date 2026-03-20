#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <atomic>
#include <mutex>

class Logger
{
public:
    enum class LogLevel : unsigned char
    {
        DEBUG = 1 << 0,
        INFO = 1 << 1,
        WARN = 1 << 2,
        ERROR = 1 << 3,
        OTHER   // 占位、无实际意义
    };
    // 异常类型定义
    struct LogException : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    std::string levelToString(const LogLevel& level) const;     // 枚举转字符串
public:
    void setFilter(unsigned char filter) { m_filter.store(filter); }      // 设置过滤器
    void log(const std::string& msg, const LogLevel& level = LogLevel::DEBUG, const std::string& module = "");   // 日志输出
    Logger& operator <<(const std::string& msg);
protected:
    Logger() = default;
    virtual ~Logger();
    virtual void logOut(const std::string& msg, const LogLevel& level, const std::string& module = "") = 0;

    // 工具函数
    static std::string getTimesTamp();

private:
    std::atomic_uchar m_filter{ 0 };  // 过滤器
    std::mutex m_mutex;             // 互斥量
};

#endif // LOGGER_H
