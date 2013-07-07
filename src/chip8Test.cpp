#include <algorithm>
#include <cstdint>
#include <vector>

#include "chip8.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"


using ::testing::Each;
using ::testing::Contains;
using ::testing::ContainerEq;
using ::testing::Eq;
using ::testing::Ne;

class chip8Test : public ::testing::Test {
protected:
  chip8 emu;
};

TEST_F(chip8Test, op_0x00E0)
{
  // set op code
  emu.memory[512]     = 0x00;
  emu.memory[512 + 1] = 0xE0;

  // fill video buffer with 1
  std::fill(emu.gfx.begin(), emu.gfx.end(), 1);

  // clear the screen
  emu.emulateCycle();

  // check and see if pc was incremented
  EXPECT_EQ(512 + 2, emu.pc);

  // check and see if gfx is cleared
  ASSERT_THAT(emu.gfx, Each(Eq(0)));
}

TEST_F(chip8Test, op_0x00EE)
{
  // set op code
  emu.memory[512]     = 0x00;
  emu.memory[512 + 1] = 0xEE;

  // set up stack
  emu.stack[emu.sp] = 0xA30;
  emu.sp++;
  emu.stack[emu.sp] = 0x200;

  // return to previous stack position
  emu.emulateCycle();

  // check and see if pc was incremented
  EXPECT_EQ(0xA32, emu.pc);

  // see if stack pointer was deincremented
  ASSERT_EQ(0, emu.sp);

  // check stack
  ASSERT_EQ(0xA30, emu.stack[emu.sp]);
}

TEST_F(chip8Test, op_0x1NNN)
{
  // set op code
  emu.memory[512]     = 0x1A;
  emu.memory[512 + 1] = 0x30;

  // jump to 0xA30
  emu.emulateCycle();

  // assert that we jumped to 0xA30, without incrementing
  ASSERT_EQ(0xA30, emu.pc);
}

TEST_F(chip8Test, op_0x2NNN)
{
  // set op code
  emu.memory[512]     = 0x2A;
  emu.memory[512 + 1] = 0x30;

  // enter subroutine at 0xA30
  emu.emulateCycle();

  // check that pc is 0xA30, without incrementing
  ASSERT_EQ(0xA30, emu.pc);

  // check that stack pointer was incremented
  ASSERT_EQ(1, emu.sp);

  // check that previous position was saved to stack
  ASSERT_EQ(0x200, emu.stack[emu.sp-1]);
}

TEST_F(chip8Test, op_0x3XNN_true)
{
  // set op code
  emu.memory[512]     = 0x3A;
  emu.memory[512 + 1] = 0x55;

  // set VX to NN
  emu.V[0xA] = 0x55;

  // if VX == NN, skip next instruction
  emu.emulateCycle();

  // check that pc was double incremented
  ASSERT_EQ(516, emu.pc);
}

TEST_F(chip8Test, op_0x3XNN_false)
{
  // set op code
  emu.memory[512]     = 0x3A;
  emu.memory[512 + 1] = 0x55;

  // set VX to !NN
  emu.V[0xA] = 0x44;

  // if VX == NN, skip next instruction
  emu.emulateCycle();

  // check that pc was single incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_4XNN_true)
{
  // set op code
  emu.memory[512]     = 0x4A;
  emu.memory[512 + 1] = 0x55;

  // set VX to !NN
  emu.V[0xA] = 0x44;

  // if VX != NN, skip next instruction
  emu.emulateCycle();

  // check that pc was double incremented
  ASSERT_EQ(516, emu.pc);
}

TEST_F(chip8Test, op_4XNN_false)
{
  // set op code
  emu.memory[512]     = 0x4A;
  emu.memory[512 + 1] = 0x55;

  // set VX to NN
  emu.V[0xA] = 0x55;

  // if VX != NN, skip next instruction
  emu.emulateCycle();

  // check that pc was single incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_5XY0_true)
{
  // set op code
  emu.memory[512]     = 0x5A;
  emu.memory[512 + 1] = 0xB0;

  // set VX = VY
  emu.V[0xA] = 0x55;
  emu.V[0xB] = 0x55;

  // if VX == VY, double increment pc
  emu.emulateCycle();

  // check that pc was doubly incremented
  ASSERT_EQ(516, emu.pc);
}

TEST_F(chip8Test, op_5XY0_false)
{
  // set op code
  emu.memory[512]     = 0x5A;
  emu.memory[512 + 1] = 0xB0;

  // set VX = VY
  emu.V[0xA] = 0x55;
  emu.V[0xB] = 0x44;

  // if VX == VY, double increment pc
  emu.emulateCycle();

  // check that pc was singly incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_6XNN)
{
  // set op code
  emu.memory[512]     = 0x6A;
  emu.memory[512 + 1] = 0x33;

  // set VX to NN
  emu.emulateCycle();

  // check that VX == NN
  ASSERT_EQ(0x33, emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_7XNN)
{
  // set op code
  emu.memory[512]     = 0x7A;
  emu.memory[512 + 1] = 0x33;

  emu.V[0xA] = 0x01;

  // add NN to VX
  emu.emulateCycle();

  // check if VX += NN
  ASSERT_EQ(0x34, emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XY0)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x30;

  emu.V[0x3] = 0x01;

  // set VX = VY
  emu.emulateCycle();

  // check VX == VY
  ASSERT_EQ(emu.V[0xA], emu.V[0x3]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XY1)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x31;

  // set VX and VY
  emu.V[0xA] = 0x01;
  emu.V[0x3] = 0xF0;

  // sets VX = vx | vy
  emu.emulateCycle();

  // check VX = VX | VY
  ASSERT_EQ(0x01 | 0xF0, emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XY2)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x32;

  // set VX and VY
  emu.V[0xA] = 0x01;
  emu.V[0x3] = 0xF0;

  // sets VX = vx & vy
  emu.emulateCycle();

  // check VX = VX & VY
  ASSERT_EQ((0x01 & 0xF0), emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XY3)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x33;

  // set VY and VY
  emu.V[0xA] = 0x01;
  emu.V[0x3] = 0xF0;

  // sets VX = vx ^ vy
  emu.emulateCycle();

  // check VX = VX ^ VY
  ASSERT_EQ(0x01 ^ 0xF0, emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XY4_carry)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x34;

  // set VX and VY
  emu.V[0xA] = 0xFE;
  emu.V[0x3] = 0x03;

  // VX += VY, VF = 1 if carry
  emu.emulateCycle();

  // check VF = 1
  ASSERT_EQ(1, emu.V[0xF]);

  // check VX += VY
  ASSERT_EQ(0x01, emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XY4_nocarry)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x34;

  // set VX and VY
  emu.V[0xA] = 0xFE;
  emu.V[0x3] = 0x01;

  // VX += VY, VF = 1 if carry
  emu.emulateCycle();

  // check VF = 0
  ASSERT_EQ(0, emu.V[0xF]);

  // check VX += VY
  ASSERT_EQ(0xFF, emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XY5_borrow)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x35;

  // set VX and VY
  emu.V[0xA] = 0x01;
  emu.V[0x3] = 0x02;

  // VX -= VY, VF = 0 if borrow
  emu.emulateCycle();

  // check VF = 0
  ASSERT_EQ(0, emu.V[0xF]);

  // check VX -= VY
  ASSERT_EQ(0xFF, emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XY5_noborrow)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x35;

  // set VX and VY
  emu.V[0xA] = 0x02;
  emu.V[0x3] = 0x01;

  // VX -= VY, VF = 0 if borrow
  emu.emulateCycle();

  // check VF = 1
  ASSERT_EQ(1, emu.V[0xF]);

  // check VX -= VY
  ASSERT_EQ(0x01, emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XY6)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x36;

  // set VX
  emu.V[0xA] = 0xAA;

  // VX >>= 1, VF = least sig bit before shift
  emu.emulateCycle();

  // VX >>= 1
  ASSERT_EQ(0x55, emu.V[0xA]);

  // VF = least sig bit
  ASSERT_EQ(0, emu.V[0xF]);
}

TEST_F(chip8Test, op_8XY7_borrow)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x37;

  // set VX and VY
  emu.V[0xA] = 0x02;
  emu.V[0x3] = 0x01;

  // VX = VY - VX, VF = 0 if borrow
  emu.emulateCycle();

  // check VF = 0
  ASSERT_EQ(0, emu.V[0xF]);

  // check VX = VY - VX
  ASSERT_EQ(0xFF, emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XY7_noborrow)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x37;

  // set VX and VY
  emu.V[0xA] = 0x01;
  emu.V[0x3] = 0x02;

  // VX = VY - VX, VF = 0 if borrow
  emu.emulateCycle();

  // check VF = 1
  ASSERT_EQ(1, emu.V[0xF]);

  // check VX = VY - VX
  ASSERT_EQ(0x01, emu.V[0xA]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_8XYE)
{
  // set op code
  emu.memory[512]     = 0x8A;
  emu.memory[512 + 1] = 0x3E;

  // set VX
  emu.V[0xA] = 0xAA;

  // VX <<= 1, VF = most sig bit before shift
  emu.emulateCycle();

  // VX <<= 1
  ASSERT_EQ(0x54, emu.V[0xA]);

  // VF = most sig bit
  ASSERT_EQ(1, emu.V[0xF]);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_9XY0_skip)
{
  // set op code
  emu.memory[512]     = 0x9A;
  emu.memory[512 + 1] = 0x30;

  // set VX == VY
  emu.V[0xA] = 0xAB;
  emu.V[0x3] = 0xAB;

  // skip next instruction if VX != VY
  emu.emulateCycle();

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_9XY0_noskip)
{
  // set op code
  emu.memory[512]     = 0x9A;
  emu.memory[512 + 1] = 0x30;

  // set VX == VY
  emu.V[0xA] = 0xAA;
  emu.V[0x3] = 0xAB;

  // skip next instruction if VX != VY
  emu.emulateCycle();

  // check that pc was doubly incremented
  ASSERT_EQ(516, emu.pc);
}

TEST_F(chip8Test, op_ANNN)
{
  // set op code
  emu.memory[512]     = 0xAA;
  emu.memory[512 + 1] = 0x30;

  // set I to memory[NNN]
  emu.emulateCycle();

  // check I = memory[NNN]
  ASSERT_EQ(0xA30, emu.I);

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_BNNN)
{
  // set op code
  emu.memory[512]     = 0xBA;
  emu.memory[512 + 1] = 0x30;

  // set V0
  emu.V[0] = 0xCC;

  // jump to address NNN plus V0
  emu.emulateCycle();

  // check pc = NNN + V0
  ASSERT_EQ((0xA30 + 0xCC), emu.pc);
}

TEST_F(chip8Test, op_0xDXYN)
{
  // set op code
  emu.memory[512]     = 0xD6;
  emu.memory[512 + 1] = 0x75;

  // set I and VX, VY
  emu.I = 0xA30;
  emu.V[0x6] = 6;
  emu.V[0x7] = 7;

  // set memory to load sprite "7"
  for(unsigned int i = 0; i < 5; i++)
    emu.memory[0xA30 + i] = emu.memory[7 * 5 + i];

  // draw 7 at pixel (6,7)
  emu.emulateCycle();

  // expect drawflag = true
  EXPECT_EQ(true, emu.drawFlag);

  // check and see if gfx is not empty
  ASSERT_THAT(emu.gfx, Contains(Ne(0)));

  // check pixels are drawn
  std::vector<int> temp{{
                        emu.gfx[6 + 7*64],
                        emu.gfx[7 + 7*64],
                        emu.gfx[8 + 7*64],
                        emu.gfx[9 + 7*64],
                        emu.gfx[9 + 8*64],
                        emu.gfx[8 + 9*64],
                        emu.gfx[7 + 10*64],
                        emu.gfx[7 + 11*64]
                      }};
  ASSERT_THAT(temp, Each(Eq(1)));

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_EX9E)
{
  // set op code
  emu.memory[512]     = 0xE3;
  emu.memory[512 + 1] = 0x9E;

  // set key pressed
  emu.key[0xA] = 0x32;
  emu.V[0x3] = 0xA;

  // skip next instruction if key in VX pressed
  emu.emulateCycle();

  // check next instruction skipped
  ASSERT_EQ(516, emu.pc);

  // set op code
  emu.memory[516]     = 0xE3;
  emu.memory[516 + 1] = 0x9E;

  // set key not pressed
  emu.key[0xA] = 0x0;

  // don't skip next instruction
  emu.emulateCycle();

  ASSERT_EQ(518, emu.pc);
}

TEST_F(chip8Test, op_EXA1)
{
  // set op code
  emu.memory[512]     = 0xE3;
  emu.memory[512 + 1] = 0xA1;

  // set key not pressed
  emu.V[0x3] = 0xA;

  // skip next instruction if key in VX not pressed
  emu.emulateCycle();

  // check next instruction skipped
  ASSERT_EQ(516, emu.pc);


  // set op code
  emu.memory[516]     = 0xE3;
  emu.memory[516 + 1] = 0xA1;

  // set key not pressed
  emu.key[0xA] = 0x32;

  // don't skip next instruction
  emu.emulateCycle();

  // don't skip next instruction
  ASSERT_EQ(518, emu.pc);
}

TEST_F(chip8Test, op_FX07)
{
  // set op code
  emu.memory[512]     = 0xF3;
  emu.memory[512 + 1] = 0x07;

  // set VX and delay_timer
  emu.V[0x3] = 0x55;
  emu.delay_timer = 0x30;

  // set VX to delay_timer
  emu.emulateCycle();

  // check VX = delay_timer
  ASSERT_EQ(0x30, emu.V[0x3]);

  // check incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_FX0A)
{
  // set op code
  emu.memory[512]     = 0xF3;
  emu.memory[512 + 1] = 0x0A;

  // run without key press
  emu.emulateCycle();

  // check pc not incremented
  ASSERT_EQ(512, emu.pc);

  // check VX not set
  ASSERT_EQ(0, emu.V[0x3]);

  // set key pressed
  emu.key[0xA] = 0x33;

  // run with key press
  emu.emulateCycle();

  // check incremented
  ASSERT_EQ(514, emu.pc);

  // check VX set
  ASSERT_EQ(0xA, emu.V[0x3]);
}

TEST_F(chip8Test, op_FX15)
{
  // set op code
  emu.memory[512]     = 0xF3;
  emu.memory[512 + 1] = 0x15;

  // set VX
  emu.V[0x3] = 0x55;

  // set delay_timer to VX
  emu.emulateCycle();

  // check delay_timer = VX (timers -= 1 at end of cycle)
  ASSERT_EQ(0x55 - 1, emu.delay_timer);

  // check incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_FX18)
{
  // set op code
  emu.memory[512]     = 0xF3;
  emu.memory[512 + 1] = 0x18;

  // set VX
  emu.V[0x3] = 0x55;

  // set sound_timer to VX
  emu.emulateCycle();

  // check sound_timer = VX (timers -= 1 at end of cycle)
  ASSERT_EQ(0x55 - 1, emu.sound_timer);

  // check incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_FX1E)
{
  // set op code
  emu.memory[512]     = 0xF3;
  emu.memory[512 + 1] = 0x1E;

  // set VX and I
  emu.V[0x3] = 0x55;
  emu.I = 0x03;

  // add VX to I
  emu.emulateCycle();

  // check I += VX
  ASSERT_EQ(0x58, emu.I);

  // check incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_FX29)
{
  // set op code
  emu.memory[512]     = 0xF3;
  emu.memory[512 + 1] = 0x29;

  // set VX
  emu.V[0x3] = 0x5;

  // set I to location of character in VX
  emu.emulateCycle();

  // check I at sprite location (each character 5 bytes long, so * 5)
  ASSERT_EQ(0x5 * 0x5, emu.I);

  // check incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_FX55)
{
  // set op code
  emu.memory[512]     = 0xF3;
  emu.memory[512 + 1] = 0x55;

  // set V0 to VX and I
  emu.V[0x0] = 0x5;
  emu.V[0x1] = 0x4;
  emu.V[0x2] = 0x3;
  emu.V[0x3] = 0x2;

  emu.I = 0xA30;

  // set I to location of character in VX
  emu.emulateCycle();

  // check V copied to memory
  for (int i = 0; i <= 0x3; i++)
    ASSERT_EQ(emu.memory[0xA30 + i], emu.V[0x0 + i]);

  // check incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_FX65)
{
  // set op code
  emu.memory[512]     = 0xF3;
  emu.memory[512 + 1] = 0x65;

  // set V0 to VX and I
  emu.memory[0xA30 + 0] = 0x5;
  emu.memory[0xA30 + 1] = 0x4;
  emu.memory[0xA30 + 2] = 0x3;
  emu.memory[0xA30 + 3] = 0x2;

  emu.I = 0xA30;

  // set I to location of character in VX
  emu.emulateCycle();

  // check memory copied to V
  for (int i = 0; i <= 0x3; i++)
    ASSERT_EQ(emu.V[0x0 + i], emu.memory[0xA30 + i]);
  
  // check incremented
  ASSERT_EQ(514, emu.pc);
}
