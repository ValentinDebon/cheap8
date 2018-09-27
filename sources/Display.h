#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#include "Types.h"

namespace Chip8 {

struct Display {

	static const Chip8::UInt8 FontSet[80];

	Display();
	virtual ~Display();

	/**
	 * Clears the screen
	 */
	void clear(void);

	/**
	 * Draws a sprite by xoring bytes values
	 * @param x Horizontal position
	 * @param y Vertical position
	 * @param length How many bytes are drawn, less than 16
	 * @param location The sprite to draw, valid memory location
	 * @return Whether a collision happened or not
	 */
	virtual bool draw(Chip8::UInt8 x, Chip8::UInt8 y, Chip8::UInt8 length, Chip8::UInt8 *location);

protected:
	Chip8::UInt64 buffer[32] = { 0 };
};

}

/* CHIP8_DISPLAY_H */
#endif
