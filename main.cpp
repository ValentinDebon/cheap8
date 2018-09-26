#include <iostream>
#include <SFML/Graphics.hpp>
#include "processor.h"

using namespace std;

int main(int argc, char** argv) {

  sf::RenderWindow window(sf::VideoMode(200,200), "cheap8");
  sf::CircleShape shape(100.f);
  shape.setFillColor(sf::Color::Green);

  Processor chip8("pong.rom");

  while(window.isOpen()) {
    sf::Event event;
    while(window.pollEvent(event)) {
      if(event.type == sf::Event::Closed) {
        window.close();
      }
    }

    chip8.emulate();

    window.clear();
    window.draw(shape);
    window.display();
  }

  return 0;

}
