#include <algorithm>
#include <cstdint>
#include <vector>

#include "chip8.h"
#include "gtest/gtest.h"

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
  std::array<std::uint8_t, 2048> temp{{0}};
  ASSERT_EQ(temp, emu.gfx);
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

TEST_F(chip8Test, op_CXNN)
{
  // rng assumes std::rand
  emu.memory[512]     = 0xCA;
  emu.memory[512 + 1] = 0x30;

  // make sure rng is in known state
  std::srand(13);
  emu.emulateCycle();

  std::srand(13);
  ASSERT_EQ(std::rand() & 0x30, emu.V[0xA]);
}

TEST_F(chip8Test, op_0xDXYN)
{
  // set op code
  emu.memory[512]     = 0xD6;
  emu.memory[512 + 1] = 0x75;

  // pixel positions
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
  std::array<std::uint8_t, 2048> expected{{0}};
#define p(x,y) expected[x + y * 64] = 1
  p(6, 7); p(7, 7 ); p(8, 7); p(9, 7);
                              p(9, 8);
                     p(8, 9);
           p(7, 10);
           p(7, 11);
#undef p

  // check that pc was incremented
  ASSERT_EQ(514, emu.pc);
}

TEST_F(chip8Test, op_0xDXYN_Wraparound)
{
  // draw 5 bytes to (V2,V3) = (62,30)
  emu.memory[512]     = 0xD2;
  emu.memory[512 + 1] = 0x35;
  emu.V[0x2] = 62;
  emu.V[0x3] = 30;

  // set memory to sprite for "E"
  emu.I = 0xA30;
  for(unsigned int i = 0; i < 5; ++i)
    emu.memory[0xA30 + i] = emu.memory[0xE * 5 + i];

  emu.emulateCycle();

  ASSERT_EQ(true, emu.drawFlag);
  ASSERT_EQ(514, emu.pc);

  // sprite should have wrapped around in both X and Y direction
  std::array<std::uint8_t, 2048> expected {{ 0 }};

#define X(x,y) expected[x + y * 64] = 1;
  X(62, 30); X(63, 30); X(0, 30); X(1, 30);
  X(62, 31);
  X(62, 0 ); X(63, 0 ); X(0, 0 ); X(1, 0 );
  X(62, 1 );
  X(62, 2 ); X(63, 2 ); X(0, 2 ); X(1, 2 );
#undef X

  ASSERT_EQ(expected, emu.gfx);
}

TEST_F(chip8Test, op_DXYN_Toggle)
{
  // draw 5 bytes to (V2,V3) = (0,0) and (V4,V5) = (2,2)
  emu.memory[512]     = 0xD2;
  emu.memory[512 + 1] = 0x35;
  emu.memory[512 + 2] = 0xD4;
  emu.memory[512 + 3] = 0x45;
  emu.V[0x2] = 0;
  emu.V[0x3] = 0;
  emu.V[0x4] = 2;
  emu.V[0x5] = 2;

  // set memory to sprites for 8 and 9
  emu.I = 0xA30;
  for(unsigned int i = 0; i < 10; ++i)
    emu.memory[0xA30 + i] = emu.memory[0x8 * 5 + i];

  // draw the 8
  emu.emulateCycle();

  ASSERT_EQ(true, emu.drawFlag);
  ASSERT_EQ(514, emu.pc);
  // I shouldn't be changed
  ASSERT_EQ(0xA30, emu.I);
  // no pixel should've been overwritten
  ASSERT_EQ(0, emu.V[0xF]);

  // sprite should have wrapped around in both X and Y direction
  std::array<std::uint8_t, 2048> expected_first {{ 0 }};
#define X(x,y) expected_first[x + y * 64] = 1;
  X(0, 0); X(1, 0); X(2, 0); X(3 ,0);
  X(0, 1);                   X(3 ,1);
  X(0, 2); X(1, 2); X(2, 2); X(3 ,2);
  X(0, 3);                   X(3 ,3);
  X(0, 4); X(1, 4); X(2, 4); X(3 ,4);
#undef X

  ASSERT_EQ(expected_first, emu.gfx);

  // now draw the 9 - since it's immediately behind the 8 in memory, we have
  // to change I to point at it
  emu.I = 0xA35;
  emu.emulateCycle();

  ASSERT_EQ(true, emu.drawFlag);
  ASSERT_EQ(516, emu.pc);
  ASSERT_EQ(0xA35, emu.I);
  // part of the 9 should've overwritten part of the 8
  ASSERT_EQ(1, emu.V[0xF]);

  std::array<std::uint8_t, 2048> expected_second(expected_first);
#define X(x,y) expected_second[x + y * 64] = 1;
#define _(x,y) expected_second[x + y * 64] = 0;
  _(2, 2); _(3, 2); X(4, 2); X(5, 2);
  X(2, 3);                   X(5, 3);
  _(2, 4); _(3, 4); X(4, 4); X(5, 4);
                             X(5, 5);
  X(2, 6); X(3, 6); X(4, 6); X(5, 6);
#undef X
#undef _

  ASSERT_EQ(expected_second, emu.gfx);
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
