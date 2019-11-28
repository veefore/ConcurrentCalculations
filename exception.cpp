#ifndef __EXCEPTION__CPP__
#define __EXCEPTION__CPP__

#include "exception.h"
#include <stdexcept>

void Ensure(bool value, const std::string& message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

void Throw(const std::string& message) {
	throw std::runtime_error(message);
}

#endif