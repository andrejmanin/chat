#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <mutex>


class Logger {
    static std::ofstream log_file;
    static std::mutex mutex;
public:
    static void openFile();

    static void msg(const std::string& message);

    static void info(const std::string& message);

    static void error(const std::string& message);

    static void warning(const std::string& message);

    static void debug(const std::string& message);

    static void fatal(const std::string& message);
};


#endif //LOGGER_H
