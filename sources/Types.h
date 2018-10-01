#ifndef CHIP8_TYPES_H
#define CHIP8_TYPES_H

namespace Chip8 {

typedef unsigned char UInt8;
typedef unsigned short UInt16;
#ifdef __x86_64__
typedef unsigned long UInt64;
#else
typedef unsigned long long UInt64;
#endif

}

/* CHIP8_TYPES_H */
#endif
