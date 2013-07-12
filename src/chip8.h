#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <string>

class chip8;

typedef void (chip8::*OpcodeWrapper)(std::uint16_t);
typedef void Opcode(std::uint16_t);

class chip8
{
public:
  // functions
  chip8();
  bool loadGame(std::string);
  void emulateCycle();
  void setKeys(std::array<std::uint8_t, 16>);
  void debugRender();

  // screen was redrawn
  bool drawFlag;
  // graphics memory
  std::array<std::uint8_t, (64 * 32)> gfx;

  // audio variable
  bool beep;

protected:
  // cpu variables
  std::uint16_t I;
  std::uint16_t pc;
  std::uint16_t sp;
  // timers
  std::uint8_t delay_timer;
  std::uint8_t sound_timer;
  std::array<std::uint16_t, 16>     stack;
  std::array<std::uint8_t,  4096>   memory;
  std::array<std::uint8_t,  16>     V;


  // input variables
  std::array<std::uint8_t, 16> key;

private:
  // opcodes
  void CLS    (std::uint16_t);
  void RET    (std::uint16_t);
  void JP_A   (std::uint16_t);
  void CALL   (std::uint16_t);
  void SE_VB  (std::uint16_t);
  void SNE_VB (std::uint16_t);
  void SE_VV  (std::uint16_t);
  void LD_VB  (std::uint16_t);
  void ADD_VB (std::uint16_t);
  void LD_VV  (std::uint16_t);
  void OR     (std::uint16_t);
  void AND    (std::uint16_t);
  void XOR    (std::uint16_t);
  void ADD_VV (std::uint16_t);
  void SUB_VV (std::uint16_t);
  void SHR    (std::uint16_t);
  void SUBN   (std::uint16_t);
  void SHL    (std::uint16_t);
  void SNE_VV (std::uint16_t);
  void LD_IA  (std::uint16_t);
  void JP_VA  (std::uint16_t);
  void RND    (std::uint16_t);
  void DRW    (std::uint16_t);
  void SKP    (std::uint16_t);
  void SKNP   (std::uint16_t);
  void LD_VDT (std::uint16_t);
  void LD_VK  (std::uint16_t);
  void LD_DTV (std::uint16_t);
  void LD_STV (std::uint16_t);
  void ADD_IV (std::uint16_t);
  void LD_FV  (std::uint16_t);
  void LD_BV  (std::uint16_t);
  void LD_IV  (std::uint16_t);
  void LD_VI  (std::uint16_t);

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
  std::array<std::map<std::uint16_t, OpcodeWrapper>, 16> opcodes{{
    { // 0x00..
      {0x0000, &chip8::CLS},
      {0x000E, &chip8::RET}
    },
    { // 0x1000
      {0x1000, &chip8::JP_A}
    },
    { // 0x2000
      {0x2000, &chip8::CALL}
    },
    { // 0x3000
      {0x3000, &chip8::SE_VB}
    },
    { // 0x4000
      {0x4000, &chip8::SNE_VB}
    },
    { // 0x5000
      {0x5000, &chip8::SE_VV}
    },
    { // 0x6000
      {0x6000, &chip8::LD_VB}
    },
    { // 0x7000
      {0x7000, &chip8::ADD_VB}
    },
    { // 0x800.
      {0x0000, &chip8::LD_VV},
      {0x0001, &chip8::OR},
      {0x0002, &chip8::AND},
      {0x0003, &chip8::XOR},
      {0x0004, &chip8::ADD_VV},
      {0x0005, &chip8::SUB_VV},
      {0x0006, &chip8::SHR},
      {0x0007, &chip8::SUBN},
      {0x000E, &chip8::SHL}
    },
    { // 0x9000
      {0x9000, &chip8::SNE_VV}
    },
    { // 0xA000
      {0xA000, &chip8::LD_IA}
    },
    { // 0xB000
      {0xB000, &chip8::JP_VA}
    },
    { // 0xC000
      {0xC000, &chip8::RND}
    },
    { // 0xD000
      {0xD000, &chip8::DRW}
    },
    { // 0xE0..
      {0x000E, &chip8::SKP},
      {0x0001, &chip8::SKNP}
    },
    { // 0xF0..
      {0x0007, &chip8::LD_VDT},
      {0x000A, &chip8::LD_VK},
      {0x0015, &chip8::LD_DTV},
      {0x0018, &chip8::LD_STV},
      {0x001E, &chip8::ADD_IV},
      {0x0029, &chip8::LD_FV},
      {0x0033, &chip8::LD_BV},
      {0x0055, &chip8::LD_IV},
      {0x0065, &chip8::LD_VI}
    }
  }};
};
#endif /* CHIP8_H */
