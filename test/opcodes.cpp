#include <algorithm>
#include <cstdint>
#include <vector>

#include "chip8.h"
#include "gtest/gtest.h"

class chip8Test : public ::testing::Test, protected chip8 { };

TEST_F(chip8Test, op_0x00E0)
{
  // set op code
  memory[512]     = 0x00;
  memory[512 + 1] = 0xE0;

  // fill video buffer with 1
  std::fill(gfx.begin(), gfx.end(), 1);

  // clear the screen
  emulateCycle();

  // check and see if pc was incremented
  EXPECT_EQ(512 + 2, pc);

  // check and see if gfx is cleared
  std::array<std::uint8_t, 2048> temp{{0}};
  ASSERT_EQ(temp, gfx);
}

TEST_F(chip8Test, op_0x00EE)
{
  // set op code
  memory[512]     = 0x00;
  memory[512 + 1] = 0xEE;

  // set up stack
  stack[sp] = 0xA30;
  sp++;
  stack[sp] = 0x200;

  // return to previous stack position
  emulateCycle();

  // check and see if pc was incremented
  EXPECT_EQ(0xA32, pc);

  // see if stack pointer was deincremented
  ASSERT_EQ(0, sp);

  // check stack
  ASSERT_EQ(0xA30, stack[sp]);
}

TEST_F(chip8Test, op_0x1NNN)
{
  // set op code
  memory[512]     = 0x1A;
  memory[512 + 1] = 0x30;

  // jump to 0xA30
  emulateCycle();

  // assert that we jumped to 0xA30, without incrementing
  ASSERT_EQ(0xA30, pc);
}

TEST_F(chip8Test, op_0x2NNN)
{
  // set op code
  memory[512]     = 0x2A;
  memory[512 + 1] = 0x30;

  // enter subroutine at 0xA30
  emulateCycle();

  // check that pc is 0xA30, without incrementing
  ASSERT_EQ(0xA30, pc);

  // check that stack pointer was incremented
  ASSERT_EQ(1, sp);

  // check that previous position was saved to stack
  ASSERT_EQ(0x200, stack[sp-1]);
}

TEST_F(chip8Test, op_0x3XNN_true)
{
  // set op code
  memory[512]     = 0x3A;
  memory[512 + 1] = 0x55;

  // set VX to NN
  V[0xA] = 0x55;

  // if VX == NN, skip next instruction
  emulateCycle();

  // check that pc was double incremented
  ASSERT_EQ(516, pc);
}

TEST_F(chip8Test, op_0x3XNN_false)
{
  // set op code
  memory[512]     = 0x3A;
  memory[512 + 1] = 0x55;

  // set VX to !NN
  V[0xA] = 0x44;

  // if VX == NN, skip next instruction
  emulateCycle();

  // check that pc was single incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_4XNN_true)
{
  // set op code
  memory[512]     = 0x4A;
  memory[512 + 1] = 0x55;

  // set VX to !NN
  V[0xA] = 0x44;

  // if VX != NN, skip next instruction
  emulateCycle();

  // check that pc was double incremented
  ASSERT_EQ(516, pc);
}

TEST_F(chip8Test, op_4XNN_false)
{
  // set op code
  memory[512]     = 0x4A;
  memory[512 + 1] = 0x55;

  // set VX to NN
  V[0xA] = 0x55;

  // if VX != NN, skip next instruction
  emulateCycle();

  // check that pc was single incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_5XY0_true)
{
  // set op code
  memory[512]     = 0x5A;
  memory[512 + 1] = 0xB0;

  // set VX = VY
  V[0xA] = 0x55;
  V[0xB] = 0x55;

  // if VX == VY, double increment pc
  emulateCycle();

  // check that pc was doubly incremented
  ASSERT_EQ(516, pc);
}

TEST_F(chip8Test, op_5XY0_false)
{
  // set op code
  memory[512]     = 0x5A;
  memory[512 + 1] = 0xB0;

  // set VX = VY
  V[0xA] = 0x55;
  V[0xB] = 0x44;

  // if VX == VY, double increment pc
  emulateCycle();

  // check that pc was singly incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_6XNN)
{
  // set op code
  memory[512]     = 0x6A;
  memory[512 + 1] = 0x33;

  // set VX to NN
  emulateCycle();

  // check that VX == NN
  ASSERT_EQ(0x33, V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_7XNN)
{
  // set op code
  memory[512]     = 0x7A;
  memory[512 + 1] = 0x33;

  V[0xA] = 0x01;

  // add NN to VX
  emulateCycle();

  // check if VX += NN
  ASSERT_EQ(0x34, V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XY0)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x30;

  V[0x3] = 0x01;

  // set VX = VY
  emulateCycle();

  // check VX == VY
  ASSERT_EQ(V[0xA], V[0x3]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XY1)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x31;

  // set VX and VY
  V[0xA] = 0x01;
  V[0x3] = 0xF0;

  // sets VX = vx | vy
  emulateCycle();

  // check VX = VX | VY
  ASSERT_EQ(0x01 | 0xF0, V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XY2)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x32;

  // set VX and VY
  V[0xA] = 0x01;
  V[0x3] = 0xF0;

  // sets VX = vx & vy
  emulateCycle();

  // check VX = VX & VY
  ASSERT_EQ((0x01 & 0xF0), V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XY3)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x33;

  // set VY and VY
  V[0xA] = 0x01;
  V[0x3] = 0xF0;

  // sets VX = vx ^ vy
  emulateCycle();

  // check VX = VX ^ VY
  ASSERT_EQ(0x01 ^ 0xF0, V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XY4_carry)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x34;

  // set VX and VY
  V[0xA] = 0xFE;
  V[0x3] = 0x03;

  // VX += VY, VF = 1 if carry
  emulateCycle();

  // check VF = 1
  ASSERT_EQ(1, V[0xF]);

  // check VX += VY
  ASSERT_EQ(0x01, V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XY4_nocarry)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x34;

  // set VX and VY
  V[0xA] = 0xFE;
  V[0x3] = 0x01;

  // VX += VY, VF = 1 if carry
  emulateCycle();

  // check VF = 0
  ASSERT_EQ(0, V[0xF]);

  // check VX += VY
  ASSERT_EQ(0xFF, V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XY5_borrow)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x35;

  // set VX and VY
  V[0xA] = 0x01;
  V[0x3] = 0x02;

  // VX -= VY, VF = 0 if borrow
  emulateCycle();

  // check VF = 0
  ASSERT_EQ(0, V[0xF]);

  // check VX -= VY
  ASSERT_EQ(0xFF, V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XY5_noborrow)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x35;

  // set VX and VY
  V[0xA] = 0x02;
  V[0x3] = 0x01;

  // VX -= VY, VF = 0 if borrow
  emulateCycle();

  // check VF = 1
  ASSERT_EQ(1, V[0xF]);

  // check VX -= VY
  ASSERT_EQ(0x01, V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XY6)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x36;

  // set VX
  V[0xA] = 0xAA;

  // VX >>= 1, VF = least sig bit before shift
  emulateCycle();

  // VX >>= 1
  ASSERT_EQ(0x55, V[0xA]);

  // VF = least sig bit
  ASSERT_EQ(0, V[0xF]);
}

TEST_F(chip8Test, op_8XY7_borrow)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x37;

  // set VX and VY
  V[0xA] = 0x02;
  V[0x3] = 0x01;

  // VX = VY - VX, VF = 0 if borrow
  emulateCycle();

  // check VF = 0
  ASSERT_EQ(0, V[0xF]);

  // check VX = VY - VX
  ASSERT_EQ(0xFF, V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XY7_noborrow)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x37;

  // set VX and VY
  V[0xA] = 0x01;
  V[0x3] = 0x02;

  // VX = VY - VX, VF = 0 if borrow
  emulateCycle();

  // check VF = 1
  ASSERT_EQ(1, V[0xF]);

  // check VX = VY - VX
  ASSERT_EQ(0x01, V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_8XYE)
{
  // set op code
  memory[512]     = 0x8A;
  memory[512 + 1] = 0x3E;

  // set VX
  V[0xA] = 0xAA;

  // VX <<= 1, VF = most sig bit before shift
  emulateCycle();

  // VX <<= 1
  ASSERT_EQ(0x54, V[0xA]);

  // VF = most sig bit
  ASSERT_EQ(1, V[0xF]);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_9XY0_skip)
{
  // set op code
  memory[512]     = 0x9A;
  memory[512 + 1] = 0x30;

  // set VX == VY
  V[0xA] = 0xAB;
  V[0x3] = 0xAB;

  // skip next instruction if VX != VY
  emulateCycle();

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_9XY0_noskip)
{
  // set op code
  memory[512]     = 0x9A;
  memory[512 + 1] = 0x30;

  // set VX == VY
  V[0xA] = 0xAA;
  V[0x3] = 0xAB;

  // skip next instruction if VX != VY
  emulateCycle();

  // check that pc was doubly incremented
  ASSERT_EQ(516, pc);
}

TEST_F(chip8Test, op_ANNN)
{
  // set op code
  memory[512]     = 0xAA;
  memory[512 + 1] = 0x30;

  // set I to memory[NNN]
  emulateCycle();

  // check I = memory[NNN]
  ASSERT_EQ(0xA30, I);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_BNNN)
{
  // set op code
  memory[512]     = 0xBA;
  memory[512 + 1] = 0x30;

  // set V0
  V[0] = 0xCC;

  // jump to address NNN plus V0
  emulateCycle();

  // check pc = NNN + V0
  ASSERT_EQ((0xA30 + 0xCC), pc);
}

TEST_F(chip8Test, op_CXNN)
{
  // rng assumes std::rand
  memory[512]     = 0xCA;
  memory[512 + 1] = 0x30;

  // make sure rng is in known state
  std::srand(13);
  emulateCycle();

  std::srand(13);
  ASSERT_EQ(std::rand() & 0x30, V[0xA]);
}

TEST_F(chip8Test, op_0xDXYN)
{
  // set op code
  memory[512]     = 0xD6;
  memory[512 + 1] = 0x75;

  // pixel positions
  I = 0xA30;
  V[0x6] = 6;
  V[0x7] = 7;

  // set memory to load sprite "7"
  for(unsigned int i = 0; i < 5; i++)
    memory[0xA30 + i] = memory[7 * 5 + i];

  // draw 7 at pixel (6,7)
  emulateCycle();

  // expect drawflag = true
  EXPECT_EQ(true, drawFlag);

  // check and see if gfx is not empty
  std::array<std::uint8_t, 2048> expected{{0}};
  expected[6 + 7*64] = 1;
  expected[7 + 7*64] = 1;
  expected[8 + 7*64] = 1;
  expected[9 + 7*64] = 1;
  expected[9 + 8*64] = 1;
  expected[8 + 9*64] = 1;
  expected[7 + 10*64] = 1;
  expected[7 + 11*64] = 1;

  ASSERT_EQ(expected, gfx);

  // check that pc was incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_EX9E)
{
  // set op code
  memory[512]     = 0xE3;
  memory[512 + 1] = 0x9E;

  // set key pressed
  key[0xA] = 0x32;
  V[0x3] = 0xA;

  // skip next instruction if key in VX pressed
  emulateCycle();

  // check next instruction skipped
  ASSERT_EQ(516, pc);

  // set op code
  memory[516]     = 0xE3;
  memory[516 + 1] = 0x9E;

  // set key not pressed
  key[0xA] = 0x0;

  // don't skip next instruction
  emulateCycle();

  ASSERT_EQ(518, pc);
}

TEST_F(chip8Test, op_EXA1)
{
  // set op code
  memory[512]     = 0xE3;
  memory[512 + 1] = 0xA1;

  // set key not pressed
  V[0x3] = 0xA;

  // skip next instruction if key in VX not pressed
  emulateCycle();

  // check next instruction skipped
  ASSERT_EQ(516, pc);


  // set op code
  memory[516]     = 0xE3;
  memory[516 + 1] = 0xA1;

  // set key not pressed
  key[0xA] = 0x32;

  // don't skip next instruction
  emulateCycle();

  // don't skip next instruction
  ASSERT_EQ(518, pc);
}

TEST_F(chip8Test, op_FX07)
{
  // set op code
  memory[512]     = 0xF3;
  memory[512 + 1] = 0x07;

  // set VX and delay_timer
  V[0x3] = 0x55;
  delay_timer = 0x30;

  // set VX to delay_timer
  emulateCycle();

  // check VX = delay_timer
  ASSERT_EQ(0x30, V[0x3]);

  // check incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_FX0A)
{
  // set op code
  memory[512]     = 0xF3;
  memory[512 + 1] = 0x0A;

  // run without key press
  emulateCycle();

  // check pc not incremented
  ASSERT_EQ(512, pc);

  // check VX not set
  ASSERT_EQ(0, V[0x3]);

  // set key pressed
  key[0xA] = 0x33;

  // run with key press
  emulateCycle();

  // check incremented
  ASSERT_EQ(514, pc);

  // check VX set
  ASSERT_EQ(0xA, V[0x3]);
}

TEST_F(chip8Test, op_FX15)
{
  // set op code
  memory[512]     = 0xF3;
  memory[512 + 1] = 0x15;

  // set VX
  V[0x3] = 0x55;

  // set delay_timer to VX
  emulateCycle();

  // check delay_timer = VX (timers -= 1 at end of cycle)
  ASSERT_EQ(0x55 - 1, delay_timer);

  // check incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_FX18)
{
  // set op code
  memory[512]     = 0xF3;
  memory[512 + 1] = 0x18;

  // set VX
  V[0x3] = 0x55;

  // set sound_timer to VX
  emulateCycle();

  // check sound_timer = VX (timers -= 1 at end of cycle)
  ASSERT_EQ(0x55 - 1, sound_timer);

  // check incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_FX1E)
{
  // set op code
  memory[512]     = 0xF3;
  memory[512 + 1] = 0x1E;

  // set VX and I
  V[0x3] = 0x55;
  I = 0x03;

  // add VX to I
  emulateCycle();

  // check I += VX
  ASSERT_EQ(0x58, I);

  // check incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_FX29)
{
  // set op code
  memory[512]     = 0xF3;
  memory[512 + 1] = 0x29;

  // set VX
  V[0x3] = 0x5;

  // set I to location of character in VX
  emulateCycle();

  // check I at sprite location (each character 5 bytes long, so * 5)
  ASSERT_EQ(0x5 * 0x5, I);

  // check incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_FX55)
{
  // set op code
  memory[512]     = 0xF3;
  memory[512 + 1] = 0x55;

  // set V0 to VX and I
  V[0x0] = 0x5;
  V[0x1] = 0x4;
  V[0x2] = 0x3;
  V[0x3] = 0x2;

  I = 0xA30;

  // set I to location of character in VX
  emulateCycle();

  // check V copied to memory
  for (int i = 0; i <= 0x3; i++)
    ASSERT_EQ(memory[0xA30 + i], V[0x0 + i]);

  // check incremented
  ASSERT_EQ(514, pc);
}

TEST_F(chip8Test, op_FX65)
{
  // set op code
  memory[512]     = 0xF3;
  memory[512 + 1] = 0x65;

  // set V0 to VX and I
  memory[0xA30 + 0] = 0x5;
  memory[0xA30 + 1] = 0x4;
  memory[0xA30 + 2] = 0x3;
  memory[0xA30 + 3] = 0x2;

  I = 0xA30;

  // set I to location of character in VX
  emulateCycle();

  // check memory copied to V
  for (int i = 0; i <= 0x3; i++)
    ASSERT_EQ(V[0x0 + i], memory[0xA30 + i]);

  // check incremented
  ASSERT_EQ(514, pc);
}
