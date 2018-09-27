#include "Interpreter.h"

#include <unistd.h>
#include <fcntl.h>
#include <strings.h>

using namespace Chip8;

static const UInt8 FontSet[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x30, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xE0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Interpreter::Interpreter(ErrorHandler *errorHandler) :
errorHandler(errorHandler) { }

Interpreter *
Interpreter::Create(const char *program, ErrorHandler *errorHandler) {
	Interpreter *interpreter = nullptr;
	int fd = open(program, O_RDONLY);

	if(fd != -1) {
		if(errorHandler == nullptr) {
			errorHandler = new ErrorHandler();
		}

		interpreter = new Interpreter(errorHandler);

		memcpy(interpreter->ram, FontSet, sizeof(FontSet));
		ssize_t written = read(fd, interpreter->ram + 0x200, sizeof(interpreter->ram));
		bzero(interpreter->ram + 0x200 + written, sizeof(interpreter->ram) - written);
		close(fd);
	}

	return interpreter;
}

bool
Interpreter::access(Chip8::UInt16 address, Chip8::UInt8 *value) {
	bool valid = address < sizeof(this->ram);

	if(valid) {
		*value = this->ram[address];
	} else {
		this->errorHandler->segmentationFault(address);
	}

	return valid;
}

bool
Interpreter::access(Chip8::UInt16 address, Chip8::UInt16 *value) {
	bool valid = address < sizeof(this->ram) - 1;

	if(valid) {
		*value = *((UInt16 *)(this->ram + address));
	} else {
		this->errorHandler->segmentationFault(address);
	}

	return valid;
}

bool
Interpreter::push(Chip8::UInt16 address) {
	bool valid = this->registers.SP < 16;

	if(valid) {
		this->registers.SP += 1;
		this->stack[this->registers.SP - 1] = address;
	} else {
		this->errorHandler->stackOverflow(this->registers.PC);
	}

	return valid;
}

bool
Interpreter::pop(Chip8::UInt16 *address) {
	bool valid = this->registers.SP > 0;

	if(valid) {
		*address = this->stack[this->registers.SP - 1];
		this->registers.SP -= 1;
	} else {
		this->errorHandler->stackUnderflow(this->registers.PC);
	}

	return valid;
}

void
Interpreter::step() {
	UInt16 instruction;

	if(this->access(this->registers.PC, &instruction)) {
		UInt16 top4 = 0xF000 & instruction;

		if(top4 < 0x8000) { // 0x0000 -> 0x7FFF
			if(top4 < 0x4000) { // 0x0000 -> 0x3FFF
				if(top4 < 0x2000) { // 0x0000 -> 0x1FFF
					if(top4 < 0x1000) { // 0x0000 -> 0x0FFF
						if(instruction == 0x00E0) {				// CLS
							// Clear the display
						} else if(instruction == 0x00EE) {			// RET
							this->pop(&this->registers.PC);
							return;
						} else {						// SYS addr
							// Ignored by modern interpreters
						}
					} else {							// JMP addr
						this->registers.PC = top4 ^ instruction;
						return;
					}
				} else { // 0x2000 -> 0x3FFF
					if(top4 == 0x2000) {						// CALL addr
						this->push(this->registers.PC);
						this->registers.PC = top4 ^ instruction;
						return;
					} else {							// SE Vx, byte
						UInt8 registerIndex = (0x0F00 & instruction) >> 8;
						UInt8 comparisonValue = (0x00FF & instruction);
						if(this->registers.V[registerIndex] == comparisonValue) {
							this->registers.PC += 1;
						}
					}
				}
			} else { // 0x4000 -> 0x7FFF
				if(top4 < 0x6000) { // 0x4000 -> 0x5FFF
				} else { // 0x6000 -> 0x7FFF
				}
			}
		} else { // 0x8000 -> 0xFFFF
			if(top4 < 0xC000) {
				if(top4 < 0xE000) {
				} else {
				}
			} else {
				if(top4 < 0xA000) {
				} else {
				}
			}
		}
	}

	this->registers.PC += 1;
}

