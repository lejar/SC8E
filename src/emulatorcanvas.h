#ifndef EMULATORCANVAS_H
#define EMULATORCANVAS_H

#include <array>
#include <string>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

class QSFMLCanvas;
class chip8;

class EmulatorCanvas : public QSFMLCanvas
{
public:

  EmulatorCanvas(QWidget*, const QPoint&, const QSize&, std::string);

private:

  void OnInit();

  void OnUpdate();

  // initialize chip8 module
  chip8 emu;

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
