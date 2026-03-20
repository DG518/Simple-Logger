#include "loggermanager.h"
#include <iostream>
// 静态成员初始化
std::unordered_map<std::string, std::shared_ptr<Logger>> LoggerManager::m_loggerMap;
std::mutex LoggerManager::m_mutex;

std::shared_ptr<Logger> LoggerManager::getConsoleLogger()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return consoleLogger();
}

std::shared_ptr<Logger> LoggerManager::getFileLogger(const std::string& fileName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // 文件名空检查
    if (fileName.empty()) {
        return consoleLogger();
    }
    // 检查是否已绑定RotateFileLogger
    const std::string rotateFileKey = getKey(LoggerType::RotateFile, fileName);
    if (m_loggerMap.find(rotateFileKey) != m_loggerMap.end()) {
        std::cerr << ("[LoggerManager][ERROR]file:" + fileName + "Bound RotateFileLogger，Cannot create FileLogger");
        return createLogger(LoggerType::Console);
    }
    // 对象存在检查
    const std::string fileKey = getKey(LoggerType::File, fileName);
    if (m_loggerMap.find(fileKey) == m_loggerMap.end()) {
        m_loggerMap[fileKey] = createLogger(LoggerType::File, fileName);
    }
    return m_loggerMap[fileKey];
}

std::shared_ptr<Logger> LoggerManager::getRotateFileLogger(const std::string& fileName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // 文件名空检查
    if (fileName.empty()) {
        return consoleLogger();
    }
    // 检查是否已绑定FileLogger
    const std::string fileKey = getKey(LoggerType::File, fileName);
    if (m_loggerMap.find(fileKey) != m_loggerMap.end()) {
        std::cerr << ("[LoggerManager][ERROR]file:" + fileName + "Bound FileLogger, Cannot create RotateFileLogger");
        return consoleLogger();
    }
    // 对象存在检查
    const std::string rotateFileKey = getKey(LoggerType::RotateFile, fileName);
    if (m_loggerMap.find(rotateFileKey) == m_loggerMap.end()) {
        m_loggerMap[rotateFileKey] = std::make_shared<RotateFileLogger>(fileName);
    }
    return m_loggerMap[rotateFileKey];
}

Logger& LoggerManager::getObject(std::shared_ptr<Logger> ptr)
{
    // 警告说明：返回临时shared_ptr的解引用，但底层对象由m_loggerMap持有（全局生命周期），无悬空风险
    if (ptr) return *(ptr.get());
    else return *(getConsoleLogger().get());
}

std::string LoggerManager::getKey(const LoggerManager::LoggerType& type, const std::string& key)
{
    switch (type) {
    case LoggerManager::LoggerType::Console:
        return "Console";
    case LoggerManager::LoggerType::File:
        return "File_" + key;
    case LoggerManager::LoggerType::RotateFile:
        return "RotateFile_" + key;
    }
    return "";
}

std::shared_ptr<Logger> LoggerManager::createLogger(const LoggerManager::LoggerType& type, const std::string& fileName)
{
    // 创建
    std::shared_ptr<Logger> logger = {};
    switch (type) {
    case LoggerManager::LoggerType::File:
        logger = std::make_shared<FileLogger>(fileName);
        break;
    case LoggerManager::LoggerType::RotateFile:
        logger = std::make_shared<RotateFileLogger>(fileName);
        break;
    default:
        logger = consoleLogger();
        break;
    }
    // 返回＋兜底
    return logger ? logger : consoleLogger();
}

std::shared_ptr<Logger> LoggerManager::consoleLogger()
{
    const std::string key = getKey(LoggerType::Console);
    if (m_loggerMap.find(key) == m_loggerMap.end()) {
        m_loggerMap[key] = std::make_shared<ConsoleLogger>();
    }
    return m_loggerMap[key];
}
