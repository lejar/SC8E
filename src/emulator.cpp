#include <array>
#include <cstdint>
#include <string>
#include <iostream>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "blip.h"
#include "chip8.h"

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
  // initialize chip8 module
  chip8 emu;

  // parse arguments
  std::string filename;  
  if (argc > 1) {
    filename = argv[1];
    if (!emu.loadGame(filename)) {
      std::cerr << "File " << filename << " does not exist!" << std::endl;
      return 1;
    }
  } else {
    std::cerr << "No filename given!" << std::endl;
    return 1;
  }

  // create window
  sf::RenderWindow window(sf::VideoMode(640, 320), "YAC8E");
  window.setFramerateLimit(120);
  sf::RectangleShape shape(sf::Vector2<float>(10, 10));
  shape.setFillColor(sf::Color::Green);

  // set up audio
  sf::SoundBuffer buffer;
  if (!buffer.loadFromMemory(blip.data(), blip.size()))
    std::cerr << "Could not load audio resources, continuing without!" << std::endl;  
  sf::Sound sound;
  sound.setBuffer(buffer);

  while (window.isOpen()) {
    // poll events
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
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
      window.clear(sf::Color(0,0,0,255));
      for (int x = 0; x < 64; x++)
        for (int y = 0; y < 32; y++)
          if (emu.gfx[x + y*64]) {
            shape.setPosition(x*10, y*10);
            window.draw(shape);
          }
    }
    window.display();
    
    // audio
    if(emu.beep)
      sound.play();
  }

  return 0;
}