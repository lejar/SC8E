#include <cstdint>
#include <iostream>

#include "blip.h"
#include "chip8.h"
#include "qsfmlcanvas.h"
#include "emulatorcanvas.h"

EmulatorCanvas::EmulatorCanvas(QWidget* Parent, const QPoint& Position, const QSize& wSize, std::string filename) :
  QSFMLCanvas(Parent, Position, wSize),
  filename(filename)
  {
  }

void EmulatorCanvas::OnInit()
  {
    // graphics
    shape.setScale(sf::Vector2<float>(10, 10));
    shape.setFillColor(sf::Color::Green);

    // sound
    if (!buffer.loadFromMemory(blip.data(), blip.size()))
      std::cerr << "Could not load audio resources, continuing without!" << std::endl;
    sound.setBuffer(buffer);

    // rom data
    if (!emu.loadGame(filename)) {
      std::cerr << "File " << filename << " does not exist!" << std::endl;
      //return 1;
    }
  }

  void EmulatorCanvas::OnUpdate()
  {
    // poll events
    sf::Event event;
    while (sf::Window::pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            sf::Window::close();
    }

    // get keys
    std::array<std::uint8_t, 16> keys;
    for (int i = 0; i < 16; i++)
      if (sf::Keyboard::isKeyPressed(layout[i]))
        keys[i] = 1;
      else
        keys[i] = 0;
    emu.setKeys(keys);

    // do cpu cycles
    emu.emulateCycle();

    // draw
    if (emu.drawFlag) {
      emu.drawFlag = false;
      sf::RenderWindow::clear(sf::Color(0,0,0,255));
      for (int x = 0; x < 64; x++)
        for (int y = 0; y < 32; y++)
          if (emu.gfx[x + y*64]) {
            shape.setPosition(x*10, y*10);
            sf::RenderWindow::draw(shape);
          }
    }

    // audio
    if(emu.beep)
      sound.play();
  }
