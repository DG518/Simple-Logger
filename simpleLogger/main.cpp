#include "LoggerManager.h"

int main(int argc, char* argv[])
{
    // ConsoleLogger
    LoggerConsole().setFilter(static_cast<unsigned char>(Logger::LogLevel::DEBUG)
        | static_cast<unsigned char>(Logger::LogLevel::WARN));
    LogConsole()->log("ConsoleLogger_Debug", Logger::LogLevel::DEBUG, __FUNCTION__);
    LogConsole()->log("ConsoleLogger_INFO",  Logger::LogLevel::INFO,  __FUNCTION__);
    LoggerConsole().log("ConsoleLogger_WARN",  Logger::LogLevel::WARN,  __FUNCTION__);
    LoggerConsole().log("ConsoleLogger_Error", Logger::LogLevel::ERROR, __FUNCTION__);
    // RotateFileLogger
    auto logger = dynamic_cast<RotateFileLogger*>(&LoggerRFile("rotateFile.log"));
    logger->setFilter(static_cast<unsigned char>(Logger::LogLevel::INFO)
        | static_cast<unsigned char>(Logger::LogLevel::ERROR));
    logger->setAllowConsole(true);
    logger->setOutLevelModule(false);
    logger->setMaxFileSize(1024 * 1024 * 5);
    logger->log("RotateFileLogger_Debug", Logger::LogLevel::DEBUG, __FUNCTION__);
    LogRFile("rotateFile.log")->log("RotateFileLogger_INFO", Logger::LogLevel::INFO, __FUNCTION__);
    LoggerRFile("rotateFile.log").log("RotateFileLogger_WARN", Logger::LogLevel::WARN, __FUNCTION__);
    LoggerRFile("rotateFile.log").log("RotateFileLogger_Error", Logger::LogLevel::ERROR, __FUNCTION__);

    return 0;
}
