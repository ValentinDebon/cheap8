#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#include "Types.h"

namespace Chip8 {

struct Display {

	Display();
	virtual ~Display();

	void clear(void);
	/**
	 * Draws a sprite by xoring bytes values
	 * @param x Horizontal position
	 * @param y Vertical position
	 * @param length How many bytes are drawn
	 * @param location The sprite to draw
	 * @return Whether a collision happened or not
	 */
	virtual bool draw(Chip8::UInt8 x, Chip8::UInt8 y, Chip8::UInt8 length, Chip8::UInt8 *location);

protected:
	Chip8::UInt64 buffer[64];
};

}

/* CHIP8_DISPLAY_H */
#endif
