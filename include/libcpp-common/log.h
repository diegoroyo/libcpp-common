/*
 * log.h
 * Diego Royo Meneses - Feb 2024
 *
 * Logging utilities
 */

#pragma once

#include <array>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

enum class LogLevel : char { DEBUG, INFO, WARNING, ERROR, __COUNT };

namespace common {

class Logger {
   private:
    static Logger m_instance;
    static Logger& instance() { return m_instance; }
    static std::ostream& output() { return *instance().m_output; }

    std::ostream* m_output;
    LogLevel m_level;
    std::mutex m_mutex;
    bool is_ansi_supported;
    bool enable_ansi;

    static void delete_previous_output() {
        if (&output() != &std::cout && &output() != &std::cerr)
            delete &output();
    }

    static std::string get_hhmmss() {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        auto now_tm = std::localtime(&now_c);
        int h = now_tm->tm_hour;
        int m = now_tm->tm_min;
        int s = now_tm->tm_sec;
        return std::string(2 - std::to_string(h).length(), '0') +
               std::to_string(h) + ':' +
               std::string(2 - std::to_string(m).length(), '0') +
               std::to_string(m) + ':' +
               std::string(2 - std::to_string(s).length(), '0') +
               std::to_string(s);
    }

    Logger() : m_output(&std::cout), m_level(LogLevel::INFO), m_mutex() {
        const char* term = std::getenv("TERM");
        is_ansi_supported =
            term != nullptr &&
            std::string(term).find("xterm") != std::string::npos;
        enable_ansi = is_ansi_supported;
    }
    ~Logger() { delete_previous_output(); }

   public:
    static void log(const LogLevel& level, const std::string& message,
                    const char* file, const int line) {
        const std::lock_guard<std::mutex> lock(instance().m_mutex);
        static const std::array<std::string, 4> level_str =  //
            {"DEBUG",                                        //
             "INFO ",                                        //
             "WARN ",                                        //
             "ERROR"};
        static const std::array<std::string, 4> color_str =  //
            {"\x1B[90m",                                     // gray
             "\x1B[97m",                                     // default
             "\x1B[1m\x1B[33m",                              // bold yellow
             "\x1B[1m\x1B[31m"};                             // bold red
        static const std::string color_rst = "\x1B[0m";

        if (level < instance().m_level) return;

        std::string hhmmss = get_hhmmss();

        output() << "[" << hhmmss << "] ";
        if (instance().enable_ansi) output() << color_str[(char)level];
        output() << level_str[(char)level] << " ";
        output() << file << ":" << line << ": ";
        output() << message << std::endl;
        if (instance().enable_ansi) output() << color_rst;
    }

    static void set_output(std::ostream& output) {
        delete_previous_output();
        instance().enable_ansi = &output == &std::cout || &output == &std::cerr;
        instance().m_output = &output;
    }
    static void set_output(const std::string& filename) {
        delete_previous_output();
        instance().enable_ansi = false;
        instance().m_output = new std::ofstream(filename);
    }
    static void set_level(LogLevel level) { instance().m_level = level; }
};

#define Log(level, message) \
    common::Logger::log(level, message, __FILE_NAME__, __LINE__)

};  // namespace common