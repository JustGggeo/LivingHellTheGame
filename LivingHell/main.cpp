#include <SFML/Graphics.hpp>

#include "GameState.h"
#include "Renderer.h"

int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Living Hell");
  window.setFramerateLimit(60);

  GameState game;
  Renderer renderer(window, "cour.ttf");

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) window.close();

      if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
        // Escape всегда закрывает
        if (key->code == sf::Keyboard::Key::Escape) {
          window.close();
          continue;
        }

        // При победе/поражении не обрабатываем ходы
        if (game.GetStatus() != GameStatus::kPlaying) continue;

        Action action = Action::kWait;
        switch (key->code) {
          case sf::Keyboard::Key::Up:
            action = Action::kMoveUp;
            break;
          case sf::Keyboard::Key::Down:
            action = Action::kMoveDown;
            break;
          case sf::Keyboard::Key::Left:
            action = Action::kMoveLeft;
            break;
          case sf::Keyboard::Key::Right:
            action = Action::kMoveRight;
            break;
          case sf::Keyboard::Key::E:
            action = Action::kUseItem;
            break;
          default:
            break;
        }
        game.ProcessAction(action);
      }
    }

    window.clear(sf::Color(20, 20, 20));
    renderer.Draw(game);
    window.display();
  }

  return 0;
}
