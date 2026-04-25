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
          case sf::Keyboard::Key::W:
            action = Action::kMoveUp;
            break;
          case sf::Keyboard::Key::S:
            action = Action::kMoveDown;
            break;
          case sf::Keyboard::Key::A:
            action = Action::kMoveLeft;
            break;
          case sf::Keyboard::Key::D:
            action = Action::kMoveRight;
            break;
          case sf::Keyboard::Key::Q:
            action = Action::kMoveUpLeft;
            break;
          case sf::Keyboard::Key::E:
            action = Action::kMoveUpRight;
            break;
          case sf::Keyboard::Key::Z:
            action = Action::kMoveDownLeft;
            break;
          case sf::Keyboard::Key::C:
            action = Action::kMoveDownRight;
            break;
          case sf::Keyboard::Key::X:
            action = Action::kUseItem;
            break;
          case sf::Keyboard::Key::Num1:
            action = Action::kUseAbility1;
            break;
          case sf::Keyboard::Key::Num2:
            action = Action::kUseAbility2;
            break;
          case sf::Keyboard::Key::Num3:
            action = Action::kUseAbility3;
            break;
          case sf::Keyboard::Key::Num4:
            action = Action::kUseAbility4;
            break;
          case sf::Keyboard::Key::Num5:
            action = Action::kUseAbility5;
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
