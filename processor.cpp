#include "processor.h"

Processor::Processor(const char* program)
  : pc(0x200), opcode(0), I(0) {

    //clear display
    int i;
    for (i = 0; i < 4096; i++) {
      memory[i] = 0;
    }
    for(i = 0; i < 16; i++) {
      stack[i] = 0;
      V[i] = 0;
      key[i] = 0;
    }
    sp = stack;

    //load font
    unsigned short chip8_fontset[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, //0
      0x20, 0x60, 0x20, 0x20, 0x70, //1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
      0xF0, 0x30, 0xF0, 0x10, 0xF0, //3
      0x90, 0x90, 0xF0, 0x10, 0x10, //4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
      0xF0, 0x10, 0x20, 0x40, 0x40, //7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
      0xF0, 0x90, 0xF0, 0x90, 0x90, //A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
      0xF0, 0x80, 0x80, 0x80, 0xF0, //C
      0xE0, 0x90, 0x90, 0x90, 0xE0, //D
      0xF0, 0x80, 0xF0, 0x80, 0xE0, //E
      0xF0, 0x80, 0xF0, 0x80, 0x80  //F
    };
    //from 0x00 to 0x50 (80)
    for(i = 0; i < 80; i++) {
      memory[i] = chip8_fontset[i];
    }
    //load program
    std::ifstream prog(program, std::ifstream::binary);
    prog.seekg(0, prog.end);
    int length = prog.tellg();
    prog.seekg(0, prog.beg);

    prog.read((char*)&memory[512], length);
    // for(i = 0 ; i < 300; i++)
    //   std::cout << memory[508 + i] << std::endl; //for debug

    functions.insert(0x0E00, instruction::cls);
    functions.insert(0x00EE, instruction::ret);
    functions.insert(0x1000, instruction::jmp);
    functions.insert(0x2000, instruction::call);
    functions.insert(0x3000, instruction::skipEqual);
    functions.insert(0x4000, instruction::skipNotEqual);
    functions.insert(0x5000, instruction::skipEqualVy);
    functions.insert(0x6000, instruction::load);
    functions.insert(0x7000, instruction::add);
    functions.insert(0x8000, instruction::loadVy);
    functions.insert(0x8001, instruction::or);
    functions.insert(0x8002, instruction::and);
    functions.insert(0x8003, instruction::xor);
    functions.insert(0x8003, instruction::xor);
    functions.insert(0x8003, instruction::xor);
    functions.insert(0x8003, instruction::xor);
    functions.insert(0x8003, instruction::xor);
    functions.insert(0x8003, instruction::xor);


}

void Processor::emulate() {
  opcode = memory[pc] << 8 | memory[pc + 1]; //opcode is two byte
  decode(opcode);
  updateTimers();
  std::cout << pc << std::endl;
}

void Processor::decode(unsigned short opcode) {
  unsigned short Fbits = opcode & 0xF000;

  switch(Fbits) {
    case 0x0000:
      if(opcode == 0x0E00) {
        for(int i = 0; i < 1388; i++) {
          gfx[i] = 0;
        }
        pc += 2;
      }
      else if(opcode == 0x00EE) {
        pc = *sp;
        sp = sp - 2;
      }
      break;
    case 0x1000:
    pc = opcode & 0x0FFF;
    break;
    case 0x2000:
      if(*sp < 15) {
        sp += 2;
        stack[*sp] = pc+2;
        pc = opcode & 0x0FFF;
      }
      break;
    case 0x3000:
      if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
        pc += 4;
      } else {
        pc += 2;
      }
      break;
    case 0x4000:
      if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
        pc += 4;
      } else {
        pc += 2;
      }
      break;
    case 0x5000:
      if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
        pc += 4;
      } else {
        pc += 2;
      }
      break;
    case 0x6000:
      V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
      pc += 2;
      break;
    case 0x7000:
      V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
      pc += 2;
      break;
    case 0x8000:
      switch(opcode & 0x000F) {
        case 0:
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
          break;
        case 1:
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
          break;
        case 2:
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
          break;
        case 3:
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
          break;
        case 4:
          V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
          //check for carry
          break;
        case 5:
          V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
          //check for borrow
          break;
        case 6:
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] >> 1;
          V[15] = V[(opcode & 0x00F0) >> 4] & 0x0001;
          break;
        case 7:
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
          //check for borrow
          break;
        case 0xE:
          V[15] = V[(opcode & 0x00F0) >> 4] & 0x8000;
          V[(opcode & 0x00F0) >> 4] = V[(opcode & 0x00F0) >> 4] << 1;
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
          break;
        default:
          std::cout << "opcode not recognized" << std::endl;
          break;
      }
      pc += 2;
      break;
    case 0x9000:
      if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
        pc += 4;
      } else {
        pc += 2;
      }
      break;
    case 0xA000:
      I = opcode & 0x0FFF;
      pc += 2;
      break;
    case 0xB000:
      pc = (opcode & 0x0FFF) + V[0];
      break;
    case 0xC000:
      srand(time(NULL));
      V[(opcode & 0x0F00) >> 8] = (rand() % 255) & (opcode & 0x00FF);
      pc += 2;
      break;
    case 0xD000:
      for(int i = 0; i < (opcode & 0x000F); i++) {
        for(int j =0; j < 8; j++) {
          gfx[(((V[opcode & 0x0F00] + i)* 64) % (64*32)) + V[opcode & 0x00F0] + j] ^= memory[I + (i*8 + j)*2];
          std::cout << "gfx indice : " << (((V[opcode & 0x0F00] + i)* 64) % (64*32)) + V[opcode & 0x00F0] + j << std::endl;
          std::cout << "memory indice : " << I + (i*8 + j)*2 << std::endl;
          std::cout << "index register : " << I << "\n";
        }
      }
      pc += 2;
      break;
    case 0xE000:
      break;
    case 0xF000:
      break;
    default:
      break;
  }
}

void Processor::updateTimers() {
  if(delay_timer > 0)
    delay_timer--;
  if(sound_timer > 0) {
    printf("beep");
    //beep
    sound_timer--;
  }
}

namespace instruction {

  void cls(unsigned short opcode) {
    if(opcode == 0x0E00) {
      for(int i = 0; i < 1388; i++) {
        gfx[i] = 0;
      }
      pc += 2;
    }
  }

  void ret(unsigned short opcode) {
    if(opcode == 0x00EE) {
      pc = *sp;
      sp = sp - 2;
    }
  }

  void jmp(unsigned short opcode) {
    pc = opcode & 0x0FFF;
  }

  void call(unsigned short opcode) {
    if(*sp < 15) {
      sp += 2;
      stack[*sp] = pc+2;
      pc = opcode & 0x0FFF;
    }
  }

  void skipEqual(unsigned short opcode) {
    if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
      pc += 4;
    } else {
      pc += 2;
    }
  }

  void skipNotEqual(unsigned short opcode) {
    if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
      pc += 4;
    } else {
      pc += 2;
    }
  }

  void skipEqualVy(unsigned short opcode) {
    if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
      pc += 4;
    } else {
      pc += 2;
    }
  }

  void load(unsigned short opcode) {
    V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
    pc += 2;
  }

  void add(unsigned short opcode) {
    V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
    pc += 2;
  }

  void loadVy(unsigned short opcode) {
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
  }

  void or(unsigned short opcode) {
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
  }

  void and(unsigned short opcode) {
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
  }

  void xor(unsigned short opcode) {
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
  }

  void addVy(unsigned short opcode) {
    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
    //check for carry
  }

  void sub(unsigned short opcode) {
    if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
      V[15] = 1;
    else
      V[15] = 0;

    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
  }

  void shr(unsigned short opcode) {
    V[15] = V[(opcode & 0x00F0) >> 4] & 0x0001;
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] >> 1;
  }

  void subn(unsigned short opcode) {
    if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
      V[15] = 1;
    else
      V[15] = 0;

    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
  }

  void shl(unsigned short opcode) {
    V[15] = V[(opcode & 0x00F0) >> 4] & 0x8000;
    V[(opcode & 0x00F0) >> 4] = V[(opcode & 0x00F0) >> 4] << 1;
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
  }

  void sne(unsigned short opcode) {
    if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
      pc += 4;
    } else {
      pc += 2;
    }
  }

  void loadI(unsigned short opcode) {
    I = opcode & 0x0FFF;
    pc += 2;
  }

  void jmpV0(unsigned short opcode) {
    pc = (opcode & 0x0FFF) + V[0];
  }

  void rand(unsigned short opcode) {
    srand(time(NULL));
    V[(opcode & 0x0F00) >> 8] = (rand() % 255) & (opcode & 0x00FF);
    pc += 2;
  }

  void display(unsigned short opcode) {
    for(int i = 0; i < (opcode & 0x000F); i++) {
      for(int j =0; j < 8; j++) {
        gfx[(((V[opcode & 0x0F00] + i)* 64) % (64*32)) + V[opcode & 0x00F0] + j] ^= memory[I + (i*8 + j)*2];
        std::cout << "gfx indice : " << (((V[opcode & 0x0F00] + i)* 64) % (64*32)) + V[opcode & 0x00F0] + j << std::endl;
        std::cout << "memory indice : " << I + (i*8 + j)*2 << std::endl;
        std::cout << "index register : " << I << "\n";
      }
    }
    pc += 2;
  }

  void skipPress(unsigned short opcode) {

  }

  void skipNotPress(unsigned short opcode) {

  }

  void loadVxDelay(unsigned short opcode) {

  }

  void loadKey(unsigned short opcode) {

  }

  void loadDelayVx(unsigned short opcode) {

  }

  void loadSTVx(unsigned short opcode) {

  }

  void addI(unsigned short opcode) {

  }

  void loadSprite(unsigned short opcode) {

  }

}
