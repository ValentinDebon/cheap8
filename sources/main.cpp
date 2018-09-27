#include "Interpreter.h"

#include <time.h>
#include <errno.h>

#include <iostream>

int
main(int argc,
	char **argv) {
	Chip8::ErrorHandler *errorHandler = new Chip8::ErrorHandler();
	Chip8::Interpreter *interpreter =
		Chip8::Interpreter::Create("tests/pong.rom", errorHandler);

	while(!errorHandler->hasErrors()) {
		struct timespec rqtp = { 0, 16666666 };
		struct timespec rmtp;

		while(nanosleep(&rqtp, &rmtp) == -1
			&& errno == EINTR) {
			rqtp = rmtp;
		}

		interpreter->step();
	}

	for(auto &error : *errorHandler) {
		std::cerr << error << std::endl;
	}

	return 0;
}
