#ifndef DERIVELOGGER_H
#define DERIVELOGGER_H

#include "logger.h"
#include <fstream>      // 文件流

/********************空日志器********************/
class NULLLogger : virtual public Logger
{
private:
    void logOut(const std::string& /*msg*/, const LogLevel& /*level*/, const std::string& /*module*/) override
    {}
};

/********************控制台日志器********************/
class ConsoleLogger : virtual public Logger
{
private:
    void logOut(const std::string& msg, const LogLevel& level, const std::string& module) override;
};

/********************文件日志器********************/
class FileLogger : virtual public Logger
{
public:
    explicit FileLogger(const std::string fileName);
    ~FileLogger() override;

    void setAllowConsole(bool allowConsole);

    void setOutLevelModule(bool outLevelModule);

protected:
    virtual void fileOut(const std::string& out);   // 文件输出函数
protected:
    std::string m_fileName = {};                    // 对象唯一绑定的文件名
    std::ofstream m_fileStream;                     // 文件流操作对象
    std::atomic_bool m_allowConsole = { true };       // 允许同时对控制台输出
    std::atomic_bool m_outLevelModule = { true };     // 输出等级模块信息
private:
    void logOut(const std::string& msg, const LogLevel& level, const std::string& module) override;
    std::mutex m_fileMutex; // 互斥量，保护文件写入
};

/********************文件轮转日志器********************/
class RotateFileLogger : virtual public FileLogger
{
public:
    explicit RotateFileLogger(const std::string fileName);

    ~RotateFileLogger() override = default;
    void setMaxFileSize(const int maxFileSize); // 设置最大大小的接口
protected:
    void rotateFile();                      // 文件轮转
protected:
    std::atomic_int m_maxFileSize = { 1024 * 1024 * 10 };    // 最大文件大小
    int m_currentFileSize = 0;                      // 当前文件大小
private:
    void fileOut(const std::string& out) override;   // 文件输出函数
private:
    std::mutex m_fileMutex; // 互斥量，保护文件写入
};

#endif // DERIVELOGGER_H