#include "logger.h"

std::ofstream Logger::log_file;
std::mutex Logger::mutex;

void Logger::openFile() {
    if(!log_file.is_open()) {
        log_file.open("/home/andrewmanin/CLionProjects/chat/server/logs.txt", std::ios::app);
        if(!log_file.is_open()) {
            throw std::runtime_error("Failed to open log file.");
        }
    }
}

void Logger::msg(const std::string& message) {
    openFile();
    try {
        std::string msg = "MESSAGE: " + message;
        log_file << message << std::endl;
    } catch (std::exception& exception) {
        log_file << "ERROR: " << exception.what() << std::endl;
    }
    log_file.close();
}

void Logger::info(const std::string& message) {
    openFile();
    try {
        std::string msg = "INFO: " + message;
        log_file << message << std::endl;
    } catch (std::exception& exception) {
        log_file << "ERROR: " << exception.what() << std::endl;
    }
    log_file.close();
}

void Logger::error(const std::string& message) {
    openFile();
    try {
        std::string msg = "ERROR: " + message;
        log_file << message << std::endl;
    } catch (std::exception& exception) {
        log_file << "ERROR: " << exception.what() << std::endl;
    }
    log_file.close();
}

void Logger::warning(const std::string& message) {
    openFile();
    try {
        std::string msg = "WARNING: " + message;
        log_file << message << std::endl;
    } catch (std::exception& exception) {
        log_file << "ERROR: " << exception.what() << std::endl;
    }
    log_file.close();
}

void Logger::debug(const std::string& message) {
    openFile();
    try {
        std::string msg = "DEBUG: " + message;
        log_file << message << std::endl;
    } catch (std::exception& exception) {
        log_file << "ERROR: " << exception.what() << std::endl;
    }
    log_file.close();
}

void Logger::fatal(const std::string& message) {
    openFile();
    try {
        std::string msg = "FATAL: " + message;
        log_file << message << std::endl;
    } catch (std::exception& exception) {
        log_file << "ERROR: " << exception.what() << std::endl;
    }
    log_file.close();
}
