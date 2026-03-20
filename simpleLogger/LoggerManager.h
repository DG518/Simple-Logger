#ifndef LOGGERMANAGER_H
#define LOGGERMANAGER_H

#include "Derivelogger.h"
#include <unordered_map>
#include <memory>

class LoggerManager
{
public:
    enum class LoggerType : unsigned char
    {
        Console, File, RotateFile
    };
public:
    static std::shared_ptr<Logger> getConsoleLogger();
    static std::shared_ptr<Logger> getFileLogger(const std::string& fileName);
    static std::shared_ptr<Logger> getRotateFileLogger(const std::string& fileName);

    static Logger& getObject(std::shared_ptr<Logger> ptr);
private:
    LoggerManager() = default;
    ~LoggerManager() = default;

    static std::string getKey(const LoggerType& type, const std::string& key = "");
    static std::shared_ptr<Logger> createLogger(const LoggerType& type, const std::string& fileName = "");
    static std::shared_ptr<Logger> consoleLogger();
    static std::unordered_map<std::string, std::shared_ptr<Logger>> m_loggerMap;
    static std::mutex m_mutex;  // 互斥量，保护对象池
};

#define LogConsole() LoggerManager::getConsoleLogger()
#define LoggerConsole() LoggerManager::getObject(LogConsole())
#define LogFile(fileName) LoggerManager::getFileLogger(fileName)
#define LoggerFile(fileName) LoggerManager::getObject(LogFile(fileName))
#define LogRFile(fileName) LoggerManager::getRotateFileLogger(fileName)
#define LoggerRFile(fileName) LoggerManager::getObject(LogRFile(fileName))

#endif // LOGGERMANAGER_H