#include "DeriveLogger.h"
#include <iostream>

/********************控制台日志器********************/
void ConsoleLogger::logOut(const std::string& msg, const Logger::LogLevel& level, const std::string& module)
{
    std::cout << "[" << module << "]" << "[" << levelToString(level) << "]" << msg << std::endl;
}

/********************文件日志器********************/
FileLogger::FileLogger(const std::string fileName)
    : m_fileName(fileName)
{
    m_fileStream.open(m_fileName, std::ios::app | std::ios::out);
    if (!m_fileStream.is_open() || !m_fileStream.good()) {
        throw LogException("[FileLogger][ERROR]file is not open:" + m_fileName);
    }
}

FileLogger::~FileLogger()
{
    if (!m_fileStream.is_open()) {
        m_fileStream.close();
    }
}

void FileLogger::logOut(const std::string& msg, const Logger::LogLevel& level, const std::string& module)
{
    if (m_allowConsole)  // 是否执行控制台输出
        std::cout << "[" << module << "]" << "[" << levelToString(level) << "]" << msg << std::endl;

    // 设置输出信息
    std::string outStr;
    if (m_outLevelModule) {
        outStr += ("[" + module + "]" + "[" + levelToString(level) + "]");
    }
    fileOut(outStr + msg);
}

void FileLogger::fileOut(const std::string& out)
{
    // 文件打开判断
    if (!m_fileStream.is_open()) {
        // 尝试open
        m_fileStream.open(m_fileName, std::ios::app | std::ios::out);
        if (!m_fileStream.is_open()) {
            throw LogException("[FileLogger][ERROR]Failed to open the file:" + m_fileName);
        }
    }
    // 写入
    std::string content = out + "," + getTimesTamp();
    std::lock_guard<std::mutex> lock(m_fileMutex);
    m_fileStream << content << std::endl;
    // 刷新
    m_fileStream.flush();
}

void FileLogger::setOutLevelModule(bool outLevelModule)
{
    m_outLevelModule.store(outLevelModule);
}

void FileLogger::setAllowConsole(bool allowConsole)
{
    m_allowConsole.store(allowConsole);
}
/********************文件轮转日志器********************/

RotateFileLogger::RotateFileLogger(const std::string fileName)
    : FileLogger(fileName)
{
}

void RotateFileLogger::rotateFile()
{
    // 关闭流
    if (m_fileStream.is_open())
        m_fileStream.close();
    m_fileStream.clear();

    // 文件存在检测
    std::ifstream checkfile(m_fileName);
    if (checkfile.good()) {
        throw LogException("[RotateFileLogger][ERROR]Rotate failed,file does not exist:" + m_fileName);
    }
    checkfile.close();

    // 重命名文件
    std::string newFileName = m_fileName + "." + getTimesTamp();
    int ret = std::rename(m_fileName.c_str(), newFileName.c_str());
    if (ret != 0) {
        throw LogException("[RotateFileLogger][ERROR]Rotate failed,rename failed:" + m_fileName);
    }
    // 打开文件
    m_fileStream.open(m_fileName, std::ios::app | std::ios::out);
    if (m_fileStream.is_open() && m_fileStream.good()) {
        throw LogException("[RotateFileLogger][ERROR]Rotate failed,open failed:" + m_fileName);
    }

    m_currentFileSize = 0;
}

void RotateFileLogger::setMaxFileSize(const int maxFileSize)
{
    m_maxFileSize.store(maxFileSize);
}

void RotateFileLogger::fileOut(const std::string& out)
{
    // 文件打开判断
    if (!m_fileStream.is_open()) {
        // 尝试open
        m_fileStream.open(m_fileName, std::ios::app | std::ios::out);
        if (!m_fileStream.is_open()) {
            throw LogException("[RotateFileLogger][ERROR]Failed to open the file:" + m_fileName);
        }
    }

    // 判断是否需要轮转
    std::string content = out + "," + getTimesTamp();
    if (m_currentFileSize + static_cast<int>(content.size()) > m_maxFileSize.load()) {
        rotateFile();   // 轮转
    }
    // 写入
    std::lock_guard<std::mutex> lock(m_fileMutex);
    m_fileStream << content << std::endl;
    // 刷新
    m_fileStream.flush();
    // 更新已写入大小
    m_currentFileSize += static_cast<int>(content.size());
}
