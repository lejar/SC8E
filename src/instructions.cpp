#include "chip8.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iterator>
#include <ios>
#include <string>

// 0x00E0 clears the screen
void chip8::CLS(std::uint16_t)
{
  std::fill(gfx.begin(), gfx.end(), 0);
  drawFlag = true;
  pc += 2;
}

// 0x00EE returns from subroutine
void chip8::RET(std::uint16_t)
{
  --sp;
  pc = stack[sp];
  pc += 2;
}

// 0x1NNN jump to address NNN
void chip8::JP_A(std::uint16_t opcode)
{
  pc = opcode & 0x0FFF;
}

// 0x2NNN jump to subroutine at NNN
void chip8::CALL(std::uint16_t opcode)
{
  stack[sp] = pc;
  ++sp;
  pc = opcode & 0x0FFF;
}

// 0x3XNN skip next instruction if vx == NN
void chip8::SE_VB(std::uint16_t opcode)
{
  if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
    pc += 4;
  else
    pc += 2;
}

// 0x4XNN skip next instruction if vx != NN
void chip8::SNE_VB(std::uint16_t opcode)
{
  if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
    pc += 4;
  else
    pc += 2;
}

// 0x5XY0 skips next instruction if VX == VY
void chip8::SE_VV(std::uint16_t opcode)
{
  if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
    pc += 4;
  else
    pc += 2;
}

// 0x6XNN sets VX to NN
void chip8::LD_VB(std::uint16_t opcode)
{
  V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
  pc += 2;
}

// 0x7XNN adds NN to VX
void chip8::ADD_VB(std::uint16_t opcode)
{
  V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
  pc += 2;
}

// 0x8XY0 sets VX to the value of VY
void chip8::LD_VV(std::uint16_t opcode)
{
  V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
  pc += 2;
}

// 0x8XY1 sets VX to VX or VY
void chip8::OR(std::uint16_t opcode)
{
  V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
  pc += 2;
}

// 0x8XY2 sets VX to VX and VY
void chip8::AND(std::uint16_t opcode)
{
  V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
  pc += 2;
}

// 0x8XY3 sets VX to VX xor VY
void chip8::XOR(std::uint16_t opcode)
{
  V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
  pc += 2;
}

// 0x8XY4 adds VY to VX. VF = 1 if carry, otherwise 0
void chip8::ADD_VV(std::uint16_t opcode)
{
  V[0xF] = (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]));
  V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
  pc += 2;
}

// 0x8XY5 VX -= VY. VF = 0 when borrow, otherwise 1
void chip8::SUB_VV(std::uint16_t opcode)
{
  V[0xF] = (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]);
  V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
  pc += 2;
}

// 0x8XY6 VX >>= 1
void chip8::SHR(std::uint16_t opcode)
{
  V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x01;
  V[(opcode & 0x0F00) >> 8] >>= 1;
  pc += 2;
}

// 0x8XY7 VX = VY - VX. VF = 0 when borrow, otherwise 1
void chip8::SUBN(std::uint16_t opcode)
{
  V[0xF] = (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]);
  V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
  pc += 2;
}

// 0x8XYE VF = VX & (0x8000 >> 15). VX <<= 1
void chip8::SHL(std::uint16_t opcode)
{
  V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x80) >> 7;
  V[(opcode & 0x0F00) >> 8] <<= 1;
  pc += 2;
}

// 0x9XY0 if VX != VY pc += 4
void chip8::SNE_VV(std::uint16_t opcode)
{
  if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
    pc += 4;
  else
    pc += 2;
}

// 0xANNN set I to NNN
void chip8::LD_IA(std::uint16_t opcode)
{
  I = (opcode & 0x0FFF);
  pc += 2;
}

// 0xBNNN jump to address NNN plus V0
void chip8::JP_VA(std::uint16_t opcode)
{
  pc = (opcode & 0x0FFF) + V[0x0];
}

// 0xCXNN set VX to random numer and NN
void chip8::RND(std::uint16_t opcode)
{
  V[(opcode & 0x0F00) >> 8] = (std::rand() % 0xFF) & (opcode & 0x00FF);
  pc += 2;
}

// 0xDXYN draw sprite at (VX, VY) if pixel change, VF = 1 otherwise 0
void chip8::DRW(std::uint16_t opcode)
{
  std::uint8_t x      = V[(opcode & 0x0F00) >> 8];
  std::uint8_t y      = V[(opcode & 0x00F0) >> 4];
  std::uint8_t height = (opcode & 0x000F);

  V[0xF] = 0;
  // run through each row
  for (int yline = 0; yline < height; yline++)
  {
    // graph the current pixel
    std::uint8_t pixel = memory[I + yline];
    // run through columns
    for (int xline = 0; xline < 8; xline++)
    {
      // if pixel filled
      if ((pixel & (0x80 >> xline)) != 0)
      {
        // sprites wrap around the screen edges
        uint pos = (x + xline) % 64 + ((y + yline) % 32) * 64;

        // see if same pixel in gfx is already filled
        if (gfx[pos] == 1)
          V[0xF] = 1;
        // switch pixel in gfx

        gfx[pos] ^= 1;
      }
    }
  }

  drawFlag = true;
  pc += 2;
}

// 0xEX9E skips next instruction if key[VX] pressed
void chip8::SKP(std::uint16_t opcode)
{
  if (key[V[(opcode & 0x0F00) >> 8]] != 0)
    pc += 4;
  else
    pc += 2;
}

// 0xEXA1 skips next instruction if key[VX] not pressed
void chip8::SKNP(std::uint16_t opcode)
{
  if (key[V[(opcode & 0x0F00) >> 8]] == 0)
    pc += 4;
  else
    pc += 2;
}

// 0xFX07 sets vx to delay_timer
void chip8::LD_VDT(std::uint16_t opcode)
{
  V[(opcode & 0x0F00) >> 8] = delay_timer;
  pc += 2;
}

// 0xFX0A wait for key press, then store in VX
void chip8::LD_VK(std::uint16_t opcode)
{
  bool pressed = false;
  for (int i = 0; i < 16; i++)
    if (key[i] != 0)
    {
      pressed = true;
      V[(opcode & 0x0F00) >> 8] = i;
    }
  if (pressed)
    pc += 2;
  else
    return;
}

// 0xFX15 set delay_timer to VX
void chip8::LD_DTV(std::uint16_t opcode)
{
  delay_timer = V[(opcode & 0x0F00) >> 8];
  pc += 2;
}

// 0xFX18 set sound_timer to VX
void chip8::LD_STV(std::uint16_t opcode)
{
  sound_timer = V[(opcode & 0x0F00) >> 8];
  pc += 2;
}

// 0xFX1E adds VX to I
void chip8::ADD_IV(std::uint16_t opcode)
{
  V[0xF] = (I + V[(opcode & 0x0F00) >> 8] > 0xFFF);
  I += V[(opcode & 0x0F00) >> 8];
  pc += 2;
}

// 0xFX29 set I to location of sprite for char in VX
void chip8::LD_FV(std::uint16_t opcode)
{
  I = V[(opcode & 0x0F00) >> 8] * 0x5;
  pc += 2;
}

// 0xFX33 store BCD of VX at I, I+1, I+2
void chip8::LD_BV(std::uint16_t opcode)
{
  memory[I]     =  V[(opcode & 0x0F00) >> 8] / 100;
  memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
  memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 10);
  pc += 2;
}

// 0xFX55 store V0 to VX in memory
void chip8::LD_IV(std::uint16_t opcode)
{
  for (int i = 0; i <= (opcode & 0x0F00) >> 8; i++)
    memory[I + i] = V[i];
  pc += 2;
}

// 0xFX65 fill V0 to VX with mem starting at I
void chip8::LD_VI(std::uint16_t opcode)
{
  for (int i = 0; i <= (opcode & 0x0F00) >> 8; i++)
    V[i] = memory[I + i];
  pc += 2;
}
