/*
 * exception.h
 * Diego Royo Meneses - Dec. 2023
 *
 * Common class for all error reporting exceptions
 */

#pragma once

#include <stdexcept>
#include <string>

namespace common {
namespace detail {

class CommonException : public std::exception {
   public:
    CommonException(const std::string& msg) : m_msg(msg) {}

    const char* what() const noexcept override { return m_msg.c_str(); }

   private:
    std::string m_msg;
};

};  // namespace detail
};  // namespace common