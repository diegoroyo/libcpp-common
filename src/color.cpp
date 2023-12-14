/*
 * color.cpp
 * Diego Royo Meneses - Dec. 2023
 *
 * Image data structure with read/write operations
 */

#include "libcpp-common/color.h"

template <>
const Color3f Color3f::Black(0, 0, 0);
template <>
const Color3f Color3f::White(1, 1, 1);
template <>
const Color3f Color3f::Red(1, 0, 0);
template <>
const Color3f Color3f::Green(0, 1, 0);
template <>
const Color3f Color3f::Blue(0, 0, 1);
template <>
const Color3f Color3f::Yellow(1, 1, 0);
template <>
const Color3f Color3f::Magenta(1, 0, 1);
template <>
const Color3f Color3f::Cyan(0, 1, 1);

template <>
const Color4f Color4f::Black(Color3f::Black);
template <>
const Color4f Color4f::White(Color3f::White);
template <>
const Color4f Color4f::Red(Color3f::Red);
template <>
const Color4f Color4f::Green(Color3f::Green);
template <>
const Color4f Color4f::Blue(Color3f::Blue);
template <>
const Color4f Color4f::Yellow(Color3f::Yellow);
template <>
const Color4f Color4f::Magenta(Color3f::Magenta);
template <>
const Color4f Color4f::Cyan(Color3f::Cyan);