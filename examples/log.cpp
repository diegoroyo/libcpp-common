#include "libcpp-common/log.h"

int main() {
    Log(LogLevel::DEBUG, "By default DEBUG is not printed");
    Log(LogLevel::INFO, "Hello, world!");
    Log(LogLevel::WARNING, "This is a warning");
    Log(LogLevel::ERROR, "This is an error");
    common::Logger::set_level(LogLevel::DEBUG);
    Log(LogLevel::DEBUG, "If we enable debug, they are printed");

    common::Logger::set_level(LogLevel::WARNING);
    Log(LogLevel::INFO, "This message should not be printed");
    Log(LogLevel::WARNING, "This message should be printed");
    Log(LogLevel::ERROR, "This message should be printed");

    common::Logger::set_output(std::cerr);
    Log(LogLevel::INFO, "This message should NOT be printed to std::cerr");
    Log(LogLevel::WARNING, "This message should be printed to std::cerr");
    Log(LogLevel::ERROR, "This message should be printed to std::cerr");

    common::Logger::set_output("test.txt");
    Log(LogLevel::INFO, "This message should NOT be printed to test.txt");
    Log(LogLevel::WARNING, "This message should be printed to test.txt");
    Log(LogLevel::ERROR, "This message should be printed to test.txt");
    return 0;
}