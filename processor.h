#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <map>

class Processor {
public:
  Processor(const char* program);
  void emulate();

private:

  void decode(unsigned short opcode);
  void updateTimers();

  unsigned short opcode;

  unsigned char memory[4096];

  unsigned char V[16]; //registers, V[15] is VF and is used for flags
  unsigned short I; //index register
  unsigned short pc; //program counter

  unsigned char gfx[64 * 32]; //display

  unsigned char delay_timer;
  unsigned char sound_timer;

  unsigned short stack[16];
  unsigned short *sp;  //stack pointer

  unsigned char key[16]; //keyboard state

  std::map<unsigned char, void (*)(unsigned char)> functions;


};

#endif
