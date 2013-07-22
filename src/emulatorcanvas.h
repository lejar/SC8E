#ifndef EMULATORCANVAS_H
#define EMULATORCANVAS_H

#include <array>
#include <string>

#include <QWidget>

#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "chip8.h"
#include "qsfmlcanvas.h"
#include "timedworker.h"

class EmulationWorker : public TimedWorker
{
  Q_OBJECT
public:
  EmulationWorker(int frequency = 60) : TimedWorker(frequency) { }
  chip8 emu;

protected:
  void tick() override {
    emu.emulateCycle();
  }
};

class EmulatorCanvas : public QSFMLCanvas
{
  Q_OBJECT
public:
  EmulatorCanvas(QWidget*);
  virtual ~EmulatorCanvas();

  bool loadFile(std::string);
  bool reloadFile();
  void setClockRate(unsigned int);
  void updateInput();

private:
  void OnInit() override;
  void OnRepaint() override;

  // emulation worker which lives on a separate thread
  EmulationWorker* worker;

  // "sprite" for drawing pixels
  sf::RectangleShape shape;

  // audio variables
  sf::SoundBuffer buffer;
  sf::Sound sound;

  // rom file name
  std::string filename;

  // input
  std::array<sf::Keyboard::Key, 16> layout{{
    sf::Keyboard::Num1,
    sf::Keyboard::Num2,
    sf::Keyboard::Num3,
    sf::Keyboard::Q,
    sf::Keyboard::W,
    sf::Keyboard::E,
    sf::Keyboard::A,
    sf::Keyboard::S,
    sf::Keyboard::D,
    sf::Keyboard::X,
    sf::Keyboard::Y,
    sf::Keyboard::C,
    sf::Keyboard::Num4,
    sf::Keyboard::R,
    sf::Keyboard::F,
    sf::Keyboard::V
  }};
};

#endif /* EMULATORCANVAS_H */
