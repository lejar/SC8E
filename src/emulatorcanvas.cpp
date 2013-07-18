#include "emulatorcanvas.h"

#include <cstdint>
#include <iostream>

#include "res/blip.h"

EmulatorCanvas::EmulatorCanvas(QWidget* Parent) :
  QSFMLCanvas(Parent)
{
  worker = new EmulationWorker();
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
}

EmulatorCanvas::~EmulatorCanvas()
{
  worker->terminate();
  worker->wait();
}

bool EmulatorCanvas::loadFile(std::string filename)
{
  this->filename = filename;
  return worker->emu.loadGame(filename);
}

bool EmulatorCanvas::reloadFile()
{
  return loadFile(filename);
}

void EmulatorCanvas::setFrameRate(unsigned int frameRate)
{
  worker->setFrameRate(frameRate);
}

void EmulatorCanvas::updateInput()
{
  // get keys
  std::array<std::uint8_t, 16> keys;
  for (int i = 0; i < 16; i++)
    keys[i] = sf::Keyboard::isKeyPressed(layout[i]);

  worker->emu.setKeys(keys);
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

  worker->start();
}

void EmulatorCanvas::OnRepaint()
{
  worker->paused = !focus;
  if (!focus) return;

  updateInput();

  // draw
  if (worker->emu.drawFlag) {
    worker->emu.drawFlag = false;

    render.clear(sf::Color::Black);

    chip8::GfxMem gfx(worker->emu.getGfxBuffer());

    for (int x = 0; x < 64; x++) {
      for (int y = 0; y < 32; y++) {
        if (gfx[x + y*64]) {
          shape.setPosition(x*10, y*10);
          render.draw(shape);
        }
      }
    }
  }

  if (worker->emu.beep)
    sound.play();
}
