#include <SFML/Graphics.hpp>

#include "GameState.h"

int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Living Hell");
  window.setFramerateLimit(60);
  GameState game;
  game.Run(window);
  return 0;
}
