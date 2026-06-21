#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "GameState.h"

int main() {
  sf::RenderWindow window(
      sf::VideoMode({static_cast<unsigned>(Constants::kWindowWidth),
                     static_cast<unsigned>(Constants::kWindowHeight)}),
      "Living Hell");
  window.setFramerateLimit(Constants::kFramerateLimit);

  bool restart = true;
  while (restart) {
    GameState game;
    restart = game.Run(window);
  }
  return 0;
}
