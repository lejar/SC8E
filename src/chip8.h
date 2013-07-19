#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <string>

class chip8;

typedef void (chip8::*OpcodeWrapper)(std::uint16_t);
typedef void Opcode(std::uint16_t);

class chip8
{
public:
  using GfxMem = std::array<std::uint8_t, 32 * 64>;

  // functions
  chip8();
  bool loadGame(std::string);
  void emulateCycle();
  void reset();
  void setKeys(std::array<std::uint8_t, 16>);
  GfxMem getGfxBuffer();

  // screen was redrawn
  bool drawFlag;

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

  // memory
  std::array<std::uint16_t, 16>     stack;
  std::array<std::uint8_t,  4096>   memory;
  std::array<std::uint8_t,  16>     V;

  // graphics memory
  GfxMem gfx;

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

  std::mutex gfxMutex;

  // bit masks with each position in the array
  // corresponding to the first byte of the opcode
  std::array<std::uint16_t, 16> masks{{
    0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF
  }};

  // function list
  std::array<std::map<std::uint16_t, OpcodeWrapper>, 16> opcodes{{
    // 0x00..
    {
      {0x00, &chip8::CLS},
      {0x0E, &chip8::RET}
    },
    // 0x1000
    { {0x00, &chip8::JP_A} },
    // 0x2000
    { {0x00, &chip8::CALL} },
    // 0x3000
    { {0x00, &chip8::SE_VB} },
    // 0x4000
    { {0x00, &chip8::SNE_VB} },
    // 0x5000
    { {0x00, &chip8::SE_VV} },
    // 0x6000
    { {0x00, &chip8::LD_VB} },
    // 0x7000
    { {0x00, &chip8::ADD_VB} },
    // 0x800.
    {
      {0x00, &chip8::LD_VV},
      {0x01, &chip8::OR},
      {0x02, &chip8::AND},
      {0x03, &chip8::XOR},
      {0x04, &chip8::ADD_VV},
      {0x05, &chip8::SUB_VV},
      {0x06, &chip8::SHR},
      {0x07, &chip8::SUBN},
      {0x0E, &chip8::SHL}
    },
    // 0x9000
    { {0x00, &chip8::SNE_VV} },
    // 0xA000
    { {0x00, &chip8::LD_IA} },
    // 0xB000
    { {0x00, &chip8::JP_VA} },
    // 0xC000
    { {0x00, &chip8::RND} },
    // 0xD000
    { {0x00, &chip8::DRW} },
    // 0xE0..
    {
      {0x01, &chip8::SKNP},
      {0x0E, &chip8::SKP}
    },
    // 0xF0..
    {
      {0x07, &chip8::LD_VDT},
      {0x0A, &chip8::LD_VK},
      {0x15, &chip8::LD_DTV},
      {0x18, &chip8::LD_STV},
      {0x1E, &chip8::ADD_IV},
      {0x29, &chip8::LD_FV},
      {0x33, &chip8::LD_BV},
      {0x55, &chip8::LD_IV},
      {0x65, &chip8::LD_VI}
    }
  }};
};
#endif /* CHIP8_H */
