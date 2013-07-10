#include "chip8.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iterator>
#include <ios>
#include <string>

chip8::chip8() : I(0), pc(0x200), sp(0), delay_timer(0), sound_timer(0),
  drawFlag(true)
{
  std::fill(gfx.begin(),    gfx.end(),    0);
  std::fill(stack.begin(),  stack.end(),  0);
  std::fill(V.begin(),      V.end(),      0);
  std::fill(memory.begin(), memory.end(), 0);
  std::fill(key.begin(),    key.end(),    0);

  std::copy(chip8_fontset.begin(), chip8_fontset.end(), memory.begin());

  std::srand(std::time(0));
}

bool chip8::loadGame(std::string filename)
{
  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open()) return false;

  auto iter = std::istreambuf_iterator<char>(file);
  auto end = std::istreambuf_iterator<char>();
  if (std::distance(iter, end) > 3583)
    return false;
  std::copy(iter, end, memory.begin() + 512);

  return true;
}

void chip8::emulateCycle()
{
  // get current opcode
  std::uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

  // handle opcode
  std::uint16_t a = (opcode & 0xF000) >> 12;
  OpcodeWrapper fn = this->opcodes[a][(opcode & masks[a])];
  (this->*fn)(opcode);

  // handle sound timers
  if (delay_timer > 0)
    --delay_timer;

  beep = false;
  if (sound_timer > 0) {
    if (sound_timer == 1)
      beep = true;
    --sound_timer;
  }

}

void chip8::setKeys(std::array<std::uint8_t, 16> keys)
{
  for (int i = 0; i < 16; i++)
    key[i] = keys[i];
}
