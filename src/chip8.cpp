#include "chip8.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iterator>
#include <ios>
#include <string>

chip8::chip8()
{
  reset();
  std::srand(std::time(0));
}

bool chip8::loadGame(std::string filename)
{
  if (filename == "") return false;
  reset();

  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file.is_open()) return false;

  // Make sure the file fits inside the memory before attempting to read it
  std::streamoff file_size = file.tellg();
  if(file_size > static_cast<std::streamoff>(memory.size() - 512))
    return false;

  file.seekg(0);
  auto start = std::istreambuf_iterator<char>(file);
  auto end = std::istreambuf_iterator<char>();

  std::copy(start, end, memory.begin() + 512);

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

void chip8::reset()
{
  drawFlag    = true;
  gfx         = {{}};
  I           = 0;
  pc          = 0x200;
  sp          = 0;
  delay_timer = 0;
  sound_timer = 0;
  stack       = {{}};
  memory      = {{}};
  V           = {{}};
  key         = {{}};

  std::copy(chip8_fontset.begin(), chip8_fontset.end(), memory.begin());
}

void chip8::setKeys(std::array<std::uint8_t, 16> keys)
{
  for (int i = 0; i < 16; i++)
    key[i] = keys[i];
}
