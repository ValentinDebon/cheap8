#ifndef CHIP8_INTERPRETER_H
#define CHIP8_INTERPRETER_H

#include "ErrorHandler.h"
#include "Display.h"

#include <random>

namespace Chip8 {

class Interpreter final {
	struct {
		Chip8::UInt8 V[16] = { 0 };
		Chip8::UInt16 I = 0;
		Chip8::UInt16 PC = 0x200;
		Chip8::UInt8 SP = 0;
	} registers;

	Chip8::UInt16 stack[16];

	Chip8::UInt8 ram[0x1000];

	struct {
		std::default_random_engine generator;
		std::uniform_int_distribution<Chip8::UInt8> distribution
			= std::uniform_int_distribution<Chip8::UInt8>(0, 255);
		inline Chip8::UInt8 operator()(void) {
			return this->distribution(generator);
		}
	} random;

	ErrorHandler *errorHandler;

	Interpreter(ErrorHandler *errorHandler);

	bool access(Chip8::UInt16 address, Chip8::UInt8 *value);
	bool access(Chip8::UInt16 address, Chip8::UInt16 *value);

	bool push(Chip8::UInt16 address);
	bool pop(Chip8::UInt16 *address);

public:
	static Interpreter *Create(const char *program, ErrorHandler *errorHandler = nullptr);

	void step(void);
};

}

/* CHIP8_INTERPRETER_H */
#endif
