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

class CommonMeshException : public CommonException {
   public:
    CommonMeshException(const std::string& msg) : CommonException(msg) {}
};

class CommonBitmapException : public CommonException {
   public:
    CommonBitmapException(const std::string& msg) : CommonException(msg) {}
};

};  // namespace detail
};  // namespace common