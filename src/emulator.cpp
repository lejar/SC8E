#include <array>
#include <cstdint>
#include <string>
#include <iostream>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <QMainWindow>

#include "chip8.h"
#include "res/blip.h"
#include "qsfmlcanvas.h"
#include "emulatorcanvas.h"

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

int main(int argc, char* argv[])
{
  /*
  // initialize chip8 module
  chip8 emu;
  */

  // parse arguments
  std::string filename;
  if (argc > 1) {
    filename = argv[1];
  } else {
    std::cerr << "No filename given!" << std::endl;
    return 1;
  }

  QApplication App(argc, argv);

  QMainWindow* MainFrame = new QMainWindow;
  MainFrame->setWindowTitle("SC8E");
  MainFrame->resize(640, 345);
  MainFrame->show();

  EmulatorCanvas* SFMLView = new EmulatorCanvas(MainFrame, QPoint(0, 25), QSize(640, 320), filename);
  SFMLView->show();

  return App.exec();

}
