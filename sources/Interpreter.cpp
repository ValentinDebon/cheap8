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
	bool valid = true;

	if(address < sizeof(this->ram)) {
		*value = this->ram[address];
	} else {
		this->errorHandler->segmentationFault(address);
		valid = false;
	}

	return valid;
}

bool
Interpreter::access(Chip8::UInt16 address, Chip8::UInt16 *value) {
	bool valid = true;

	if(address < sizeof(this->ram)) {
		*value = *((UInt16 *)(this->ram + address));
	} else {
		this->errorHandler->segmentationFault(address);
		valid = false;
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
						} else if(instruction == 0x00EE) {		// RET
							// Return from subroutine
						} else {								// SYS addr
							// Ignored by modern interpreters
						}
					} else {									// JMP addr
						this->registers.PC = top4 ^ instruction;
						return;
					}
				} else { // 0x2000 -> 0x3FFF
					if(top4 == 0x2000) {						// CALL addr
						// Call subroutine
					} else {									// SE Vx, byte
						if(this->registers.V[(0x0F00 & instruction) >> 8] == (0x00FF & instruction) {
							this->registers.PC += 1;
						}
					}
				}
			} else {
				if(top4 < 0x6000) {
				} else {
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
}

