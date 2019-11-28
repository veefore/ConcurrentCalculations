#ifndef __EXCEPTION__H__
#define __EXCEPTION__H__

#include <stdexcept>

void Ensure(bool value, const std::string& message = "");

void Throw(const std::string& message = "");

#endif