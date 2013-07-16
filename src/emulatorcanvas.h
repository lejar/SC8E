#ifndef EMULATORCANVAS_H
#define EMULATORCANVAS_H

#include <array>
#include <string>

#include <QThread>
#include <QWidget>

#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "qsfmlcanvas.h"
#include "chip8.h"

class EmulatorCanvas : public QSFMLCanvas
{
  Q_OBJECT
public:
  EmulatorCanvas(QWidget*);
  virtual ~EmulatorCanvas();

  bool loadFile(std::string);
  void setFrameRate(unsigned int);

  // initialize chip8 module
  chip8 emu;

private slots:
  void cpuTick();

private:
  void OnInit() override;
  void OnRepaint() override;

  // timer for actual emulation cycles
  QThread cpuThread;
  QTimer cpuTimer;
  unsigned int frameRate;

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
