#ifndef CHIP8_ERROR_HANDLER_H
#define CHIP8_ERROR_HANDLER_H

#include "Types.h"

#include <deque>
#include <string>

namespace Chip8 {

class ErrorHandler {
	std::deque<std::string> errors;
	void queueError(std::string error);

public:
	ErrorHandler();
	virtual ~ErrorHandler(void);

	bool hasErrors() const;
	inline std::deque<std::string>::iterator begin(void) { return this->errors.begin(); }
	inline std::deque<std::string>::iterator end(void) { return this->errors.end(); }
	inline std::deque<std::string>::const_iterator cbegin(void) const { return this->errors.cbegin(); }
	inline std::deque<std::string>::const_iterator cend(void) const { return this->errors.cend(); }
	virtual void segmentationFault(Chip8::UInt16 address);
	virtual void stackOverflow(Chip8::UInt16 PC);
	virtual void stackUnderflow(Chip8::UInt16 PC);
};

}

/* CHIP8_ERROR_HANDLER_H */
#endif
