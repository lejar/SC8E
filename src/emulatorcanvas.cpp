#include "emulatorcanvas.h"

#include <cstdint>
#include <iostream>

#include "res/blip.h"

EmulatorCanvas::EmulatorCanvas(QWidget* Parent) :
  QSFMLCanvas(Parent)
{
}

bool EmulatorCanvas::loadFile(std::string filename)
{
  this->filename = filename;
  return emu.loadGame(filename);
}

void EmulatorCanvas::OnInit()
{
  // graphics
  shape.setSize(sf::Vector2<float>(10, 10));
  shape.setFillColor(sf::Color::Green);

  // sound
  if (!buffer.loadFromMemory(blip.data(), blip.size()))
    std::cerr << "Could not load audio resources, continuing without!" << std::endl;
  else
    sound.setBuffer(buffer);
}

void EmulatorCanvas::OnUpdate()
{
  if (!focus) return;

  // get keys
  std::array<std::uint8_t, 16> keys;
  for (int i = 0; i < 16; i++)
    keys[i] = sf::Keyboard::isKeyPressed(layout[i]);
  emu.setKeys(keys);

  // do cpu cycles
  emu.emulateCycle();

  // draw
  if (emu.drawFlag) {
    emu.drawFlag = false;

    render.clear(sf::Color(0,0,0,255));

    for (int x = 0; x < 64; x++) {
      for (int y = 0; y < 32; y++) {
        if (emu.gfx[x + y*64]) {
          shape.setPosition(x*10, y*10);
          render.draw(shape);
        }
      }
    }
  }

  // audio
  if(emu.beep)
    sound.play();
}
