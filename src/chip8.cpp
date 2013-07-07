#include "chip8.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <ios>
#include <string>

chip8::chip8()
{
  pc = 0x200;
  opcode = 0;
  I = 0;
  sp = 0;
  drawFlag = true;

  std::fill(gfx.begin(),    gfx.end(),    0);
  std::fill(stack.begin(),  stack.end(),  0);
  std::fill(V.begin(),      V.end(),      0);
  std::fill(memory.begin(), memory.end(), 0);
  std::fill(key.begin(), key.end(), 0);

  std::copy(chip8_fontset.begin(), chip8_fontset.end(), memory.begin());

  delay_timer = 0;
  sound_timer = 0;

  std::srand(std::time(0));
}

bool chip8::loadGame(std::string filename)
{
  std::ifstream file;
  file.open(filename, std::ios::binary);

  if (file.is_open()) {
    file.seekg(0, std::ios::end);
    int file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    file.read((char*)(memory.begin() + 512), file_size);
    
    file.close();
    return true;
  }
  return false;
}

void chip8::emulateCycle()
{
  // get current opcode
  opcode = (memory[pc] << 8) | memory[pc + 1];

  // handle opcode
  switch (opcode & 0xF000) {
    case 0x0000:
      switch (opcode & 0x000F) {
        // 0x00E0 clears the screen
        case 0x0000:
          std::fill(gfx.begin(), gfx.end(), 0);
          drawFlag = true;
          pc += 2;
          break;

        // 0x00EE returns from subroutine
        case 0x000E:
          --sp;
          pc = stack[sp];
          pc += 2;
          break;
      }
      break;

    // 0x1NNN jump to address NNN
    case 0x1000:
      pc = opcode & 0x0FFF;
      break;
    
    // 0x2NNN jump to subroutine at NNN
    case 0x2000:
      stack[sp] = pc;
      ++sp;
      pc = opcode & 0x0FFF;
      break;

    // 0x3XNN skip next instruction if vx == NN
    case 0x3000:
      if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        pc += 4;
      else
        pc += 2;
      break;

    // 0x4XNN skip next instruction if vx != NN
    case 0x4000:
      if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        pc += 4;
      else
        pc += 2;
      break;

    // 0x5XY0 skips next instruction if VX == VY
    case 0x5000:
      if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
        pc += 4;
      else
        pc += 2;
      break;

    // 0x6XNN sets VX to NN
    case 0x6000:
      V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
      pc += 2;
      break;

    // 0x7XNN adds NN to VX
    case 0x7000:
      V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
      pc += 2;
      break;

    case 0x8000:
      switch (opcode & 0x000F) {
        // 0x8XY0 sets VX to the value of VY
        case 0x0000:
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;

        // 0x8XY1 sets VX to VX or VY
        case 0x0001:
          V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;

        // 0x8XY2 sets VX to VX and VY
        case 0x0002:
          V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;

        // 0x8XY3 sets VX to VX xor VY
        case 0x0003:
          V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;

        // 0x8XY4 adds VY to VX. VF = 1 if carry, otherwise 0
        case 0x0004:
          V[0xF] = (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]));
          V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;

        // 0x8XY5 VX -= VY. VF = 0 when borrow, otherwise 1
        case 0x0005:
          V[0xF] = (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]);
          V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;

        // VF = VX & 0x0001. 0x8XY6 VX >>= 1
        case 0x0006:
          V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x01;
          V[(opcode & 0x0F00) >> 8] >>= 1;
          pc += 2;
          break;

        // 0x8XY7 VX = VY - VX. VF = 0 when borrow, otherwise 1
        case 0x0007:
          V[0xF] = (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]);
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        // 0x8XYE VF = VX & (0x8000 >> 15). VX <<= 1
        case 0x000E:
          V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x80) >> 7;
          V[(opcode & 0x0F00) >> 8] <<= 1;
          pc += 2;
          break;
      }
      break;

    // 0x9XY0 if VX != VY pc += 4
    case 0x9000:
      if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
        pc += 4;
      else
        pc += 2;
      break;

    // 0xANNN set I to NNN
    case 0xA000:
      I = (opcode & 0x0FFF);
      pc += 2;
      break;

    // 0xBNNN jump to address NNN plus V0
    case 0xB000:
      pc = (opcode & 0x0FFF) + V[0x0];
      break;

    // 0xCXNN set VX to random numer and NN
    case 0xC000:
      V[(opcode & 0x0F00) >> 8] = (std::rand() % 0xFF) & (opcode & 0x00FF);
      pc += 2;
      break;

    // 0xDXYN draw sprite at (VX, VY) if pixel change, VF = 1 otherwise 0
    case 0xD000: {
      std::uint8_t x      = V[(opcode & 0x0F00) >> 8];
      std::uint8_t y      = V[(opcode & 0x00F0) >> 4];
      std::uint8_t height = (opcode & 0x000F);
      std::uint8_t pixel;

      V[0xF] = 0;
      // run through each row
      for (int yline = 0; yline < height; yline++)
      {
        // graph the current pixel
        pixel = memory[I + yline];
        // run through columns
        for (int xline = 0; xline < 8; xline++)
        {
          // if pixel filled
          if ((pixel & (0x80 >> xline)) != 0)
          {
            // see if same pixel in gfx is already filled
            if (gfx[x + xline + ((y + yline) * 64)] == 1)
              V[0xF] = 1;
            // switch pixel in gfx
            gfx[x + xline + ((y + yline) * 64)] ^= 1;
          }
        }
      }

      drawFlag = true;
      pc += 2;
      break;
    }

    case 0xE000:
      switch (opcode & 0x000F) {
        // 0xEX9E skips next instruction if key[VX] pressed
        case 0x000E:
          if (key[V[(opcode & 0x0F00) >> 8]] != 0)
            pc += 4;
          else
            pc += 2;
          break;

        // 0xEXA1 skips next instruction if key[VX] not pressed
        case 0x0001:
          if (key[V[(opcode & 0x0F00) >> 8]] == 0)
            pc += 4;
          else
            pc += 2;
          break;
      }
      break;

    case 0xF000:
      switch (opcode & 0x00FF) {
        // 0xFX07 sets vx to delay_timer
        case 0x0007:
          V[(opcode & 0x0F00) >> 8] = delay_timer;
          pc += 2;
          break;

        // 0xFX0A wait for key press, then store in VX
        case 0x000A: {
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
          break;
        }

        // 0xFX15 set delay_timer to VX
        case 0x0015:
          delay_timer = V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        // 0xFX18 set sound_timer to VX
        case 0x0018:
          sound_timer = V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        // 0xFX1E adds VX to I
        case 0x001E:
          V[0xF] = (I + V[(opcode & 0x0F00) >> 8] > 0xFFF);
          I += V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        // 0xFX29 set I to location of sprite for char in VX
        case 0x0029:
          I = V[(opcode & 0x0F00) >> 8] * 0x5;
          pc += 2;
          break;

        // 0xFX33 store BCD of VX at I, I+1, I+2
        case 0x0033:
          memory[I]     =  V[(opcode & 0x0F00) >> 8] / 100;
          memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
          memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
          pc += 2;
          break;

        // 0xFX55 store V0 to VX in memory
        case 0x0055:
          for (int i = 0; i <= (opcode & 0x0F00) >> 8; i++)
            memory[I + i] = V[i];
          pc += 2;
          break;

        // 0xFX65 fill V0 to VX with mem starting at I
        case 0x0065:
          for (int i = 0; i <= (opcode & 0x0F00) >> 8; i++)
            V[i] = memory[I + i];
          pc += 2;
          break;
      }
      break;
  }

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
