#include "Interpreter.h"

#include <unistd.h>
#include <fcntl.h>
#include <strings.h>

using namespace Chip8;

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

		bcopy(Display::FontSet, interpreter->ram, sizeof(Display::FontSet));
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

namespace Instruction {

static inline UInt8
X(UInt16 instruction) {
	return (0x0F00 & instruction) >> 8;
}

static inline UInt8
Y(UInt16 instruction) {
	return (0x00F0 & instruction) >> 4;
}

static inline UInt8
KK(UInt16 instruction) {
	return 0x00FF & instruction;
}

}

void
Interpreter::step() {
	UInt16 instruction;

	if(this->access(this->registers.PC, &instruction)) {
		UInt16 opcode = 0xF000 & instruction;

		if(opcode < 0x8000) { // 0x0000 -> 0x7FFF
			if(opcode < 0x4000) { // 0x0000 -> 0x3FFF
				if(opcode < 0x2000) { // 0x0000 -> 0x1FFF
					if(opcode < 0x1000) { // 0x0000 -> 0x0FFF
						if(instruction == 0x00E0) {				// CLS
							// Clear the display
						} else if(instruction == 0x00EE) {			// RET
							this->pop(&this->registers.PC);
							return;
						} else {						// SYS addr
							// Ignored by modern interpreters
						}
					} else {							// JMP addr
						this->registers.PC = opcode ^ instruction;
						return;
					}
				} else { // 0x2000 -> 0x3FFF
					if(opcode == 0x2000) {						// CALL addr
						this->push(this->registers.PC);
						this->registers.PC = opcode ^ instruction;
						return;
					} else {							// SE Vx, byte
						UInt8 registerX = Instruction::X(instruction);
						UInt8 value = Instruction::KK(instruction);
						if(this->registers.V[registerX] == value) {
							this->registers.PC += 1;
						}
					}
				}
			} else { // 0x4000 -> 0x7FFF
				if(opcode < 0x6000) { // 0x4000 -> 0x5FFF
					if(opcode == 0x4000) {						// SNE Vx, byte
						UInt8 registerX = Instruction::X(instruction);
						UInt8 value = Instruction::KK(instruction);
						if(this->registers.V[registerX] != value) {
							this->registers.PC += 1;
						}
					} else if((0xF00F & instruction) == 0x5000) {			// SE Vx, Vy
						UInt8 registerX = Instruction::X(instruction);
						UInt8 registerY = Instruction::Y(instruction);
						if(this->registers.V[registerX] == this->registers.V[registerY]) {
							this->registers.PC += 1;
						}
					}
				} else { // 0x6000 -> 0x7FFF
					if(opcode == 0x6000) {						// LD Vx, byte
						UInt8 registerX = Instruction::X(instruction);
						this->registers.V[registerX] = Instruction::KK(instruction);
					} else {							// ADD Vx, byte
						UInt8 registerX = Instruction::X(instruction);
						this->registers.V[registerX] += Instruction::KK(instruction);
					}
				}
			}
		} else { // 0x8000 -> 0xFFFF
			if(opcode < 0xC000) { // 0x8000 -> 0xBFFF
				if(opcode < 0xA000) { // 0x8000 -> 0x9FFF
					if(opcode == 0x8000) {
						opcode = (0xF00F & instruction);
					} else {
					}
				} else { // 0xA000 -> 0xBFFF
					if(opcode == 0xA000) {						// LD I, addr
						this->registers.I = opcode ^ instruction;
					} else {							// JP V0, addr
						this->registers.PC = (opcode ^ instruction) + this->registers.V[0];
					}
				}
			} else { // 0xC000 -> 0xFFFF
				if(opcode < 0xE000) { // 0xC000 -> 0xDFFF
					if(opcode == 0xC000) {						// RND Vx, byte
						this->registers.V[Instruction::X(instruction)]
							= this->random() & Instruction::KK(instruction);
					} else {							// DRW Vx, Vy, nibble
					}
				} else { // 0xE000 -> 0xFFFF
					if(opcode == 0xE000) {
					} else {
					}
				}
			}
		}
	}

	this->registers.PC += 1;
}

