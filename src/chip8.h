#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <string>

class chip8
{
public:
  // cpu variables
  std::uint16_t I;
  std::uint16_t pc;
  std::uint16_t sp;
  std::array<std::uint16_t, 16>     stack;
  std::array<std::uint8_t,  4096>   memory;
  std::array<std::uint8_t,  16>     V;


  // timers
  std::uint8_t delay_timer;
  std::uint8_t sound_timer;

  // graphics variables
  std::array<std::uint8_t, (64 * 32)> gfx;
  bool drawFlag;

  // audio variable
  bool beep;

  // input variables
  std::array<std::uint8_t, 16> key;

  // functions
  chip8();
  bool loadGame(std::string);
  void emulateCycle();
  void setKeys(std::array<std::uint8_t, 16>);
  void debugRender();

  // opcodes
  void CLS(std::uint16_t);
  void RET(std::uint16_t);
  void JP_A(std::uint16_t);
  void CALL(std::uint16_t);
  void SE_VB(std::uint16_t);
  void SNE_VB(std::uint16_t);
  void SE_VV(std::uint16_t);
  void LD_VB(std::uint16_t);
  void ADD_VB(std::uint16_t);
  void LD_VV(std::uint16_t);
  void OR(std::uint16_t);
  void AND(std::uint16_t);
  void XOR(std::uint16_t);
  void ADD_VV(std::uint16_t);
  void SUB_VV(std::uint16_t);
  void SHR(std::uint16_t);
  void SUBN(std::uint16_t);
  void SHL(std::uint16_t);
  void SNE_VV(std::uint16_t);
  void LD_IA(std::uint16_t);
  void JP_VA(std::uint16_t);
  void RND(std::uint16_t);
  void DRW(std::uint16_t);
  void SKP(std::uint16_t);
  void SKNP(std::uint16_t);
  void LD_VDT(std::uint16_t);
  void LD_VK(std::uint16_t);
  void LD_DTV(std::uint16_t);
  void LD_STV(std::uint16_t);
  void ADD_IV(std::uint16_t);
  void LD_FV(std::uint16_t);
  void LD_BV(std::uint16_t);
  void LD_IV(std::uint16_t);
  void LD_VI(std::uint16_t);

  // static variables
  // font set - constains the sprites for drawing characters
  std::array<std::uint8_t, 80> chip8_fontset{{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
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
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  }};

  // bit masks with each position in the array
  // corresponding to the first byte of the opcode
  std::array<std::uint16_t, 16> masks{{
    0x000F,
    0xF000,
    0xF000,
    0xF000,
    0xF000,
    0xF000,
    0xF000,
    0xF000,
    0x000F,
    0xF000,
    0xF000,
    0xF000,
    0xF000,
    0xF000,
    0x000F,
    0x00FF
  }};

  // function list
  std::array<std::map<std::uint16_t, std::function<void(std::uint16_t)>>, 16> opcodes{{
    { // 0x00..
      {0x0000, std::bind(&chip8::CLS, this, std::placeholders::_1)},
      {0x000E, std::bind(&chip8::RET, this, std::placeholders::_1)}
    },
    { // 0x1000
      {0x1000, std::bind(&chip8::JP_A, this, std::placeholders::_1)}
    },
    { // 0x2000
      {0x2000, std::bind(&chip8::CALL, this, std::placeholders::_1)}
    },
    { // 0x3000
      {0x3000, std::bind(&chip8::SE_VB, this, std::placeholders::_1)}
    },
    { // 0x4000
      {0x4000, std::bind(&chip8::SNE_VB, this, std::placeholders::_1)}
    },
    { // 0x5000
      {0x5000, std::bind(&chip8::SE_VV, this, std::placeholders::_1)}
    },
    { // 0x6000
      {0x6000, std::bind(&chip8::LD_VB, this, std::placeholders::_1)}
    },
    { // 0x7000
      {0x7000, std::bind(&chip8::ADD_VB, this, std::placeholders::_1)}
    },
    { // 0x800.
      {0x0000, std::bind(&chip8::LD_VV, this, std::placeholders::_1)},
      {0x0001, std::bind(&chip8::OR, this, std::placeholders::_1)},
      {0x0002, std::bind(&chip8::AND, this, std::placeholders::_1)},
      {0x0003, std::bind(&chip8::XOR, this, std::placeholders::_1)},
      {0x0004, std::bind(&chip8::ADD_VV, this, std::placeholders::_1)},
      {0x0005, std::bind(&chip8::SUB_VV, this, std::placeholders::_1)},
      {0x0006, std::bind(&chip8::SHR, this, std::placeholders::_1)},
      {0x0007, std::bind(&chip8::SUBN, this, std::placeholders::_1)},
      {0x000E, std::bind(&chip8::SHL, this, std::placeholders::_1)}
    },
    { // 0x9000
      {0x9000, std::bind(&chip8::SNE_VV, this, std::placeholders::_1)}
    },
    { // 0xA000
      {0xA000, std::bind(&chip8::LD_IA, this, std::placeholders::_1)}
    },
    { // 0xB000
      {0xB000, std::bind(&chip8::JP_VA, this, std::placeholders::_1)}
    },
    { // 0xC000
      {0xC000, std::bind(&chip8::RND, this, std::placeholders::_1)}
    },
    { // 0xD000
      {0xD000, std::bind(&chip8::DRW, this, std::placeholders::_1)}
    },
    { // 0xE0..
      {0x000E, std::bind(&chip8::SKP, this, std::placeholders::_1)},
      {0x0001, std::bind(&chip8::SKNP, this, std::placeholders::_1)}
    },
    { // 0xF0..
      {0x0007, std::bind(&chip8::LD_VDT, this, std::placeholders::_1)},
      {0x000A, std::bind(&chip8::LD_VK, this, std::placeholders::_1)},
      {0x0015, std::bind(&chip8::LD_DTV, this, std::placeholders::_1)},
      {0x0018, std::bind(&chip8::LD_STV, this, std::placeholders::_1)},
      {0x001E, std::bind(&chip8::ADD_IV, this, std::placeholders::_1)},
      {0x0029, std::bind(&chip8::LD_FV, this, std::placeholders::_1)},
      {0x0033, std::bind(&chip8::LD_BV, this, std::placeholders::_1)},
      {0x0055, std::bind(&chip8::LD_IV, this, std::placeholders::_1)},
      {0x0065, std::bind(&chip8::LD_VI, this, std::placeholders::_1)}
    }
  }};
};
#endif /* CHIP8_H */
