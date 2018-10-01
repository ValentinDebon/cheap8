#include "ErrorHandler.h"

using namespace Chip8;

ErrorHandler::ErrorHandler() :
errors() { }

ErrorHandler::~ErrorHandler() { }

void
ErrorHandler::queueError(std::string error) {
	this->errors.push_back(error);
}

bool
ErrorHandler::hasErrors() const {
	return !this->errors.empty();
}

void
ErrorHandler::segmentationFault(UInt16 address) {
	this->queueError(std::string("Invalid access of memory at address: ")
		+ std::to_string(address));
}

void
ErrorHandler::stackOverflow(Chip8::UInt16 PC) {
	this->queueError(std::string("Stack overflow at PC: ")
		+ std::to_string(PC));
}

void
ErrorHandler::stackUnderflow(Chip8::UInt16 PC) {
	this->queueError(std::string("Stack underflow at PC: ")
		+ std::to_string(PC));
}

