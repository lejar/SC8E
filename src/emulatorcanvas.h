#ifndef EMULATORCANVAS_H
#define EMULATORCANVAS_H

#include <array>
#include <string>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <QMainWindow>
#include <QObject>
#include <QMenuBar>

#include "qsfmlcanvas.h"
#include "emulatormenu.h"
#include "chip8.h"

class EmulatorCanvas : public QSFMLCanvas, private Ui_MenuBar
{
public:

  EmulatorCanvas(QMainWindow*, const QPoint&, const QSize&, std::string);

  EmulatorMenu* menuBar;

  // initialize chip8 module
  chip8 emu;

private:

  void OnInit();

  void OnUpdate();


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
