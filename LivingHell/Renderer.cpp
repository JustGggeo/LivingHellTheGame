#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow& window, const std::string& font_path)
    : window_(window), tile_size_(20.f) {
  font_.openFromFile(font_path);
}

void Renderer::Draw(GameState& game) {
  DrawRoom(game.GetCurrentRoom());
  DrawPlayer(game.GetPlayer());
  DrawEnemies(game.GetCurrentRoom());
  DrawUI(game);
}

void Renderer::DrawRoom(Room& room) {
  for (int y = 0; y < room.GetHeight(); y++) {
    for (int x = 0; x < room.GetWidth(); x++) {
      Tile& tile = room.GetTile(x, y);
      std::string symbol = ".";
      sf::Color color = sf::Color(80, 80, 80);
      switch (tile.GetType()) {
        case TileType::kWall:
          symbol = "#";
          color = sf::Color(180, 180, 180);
          break;
        case TileType::kDoor:
          symbol = "+";
          color = sf::Color(200, 150, 50);
          break;
        case TileType::kExit:
          symbol = ">";
          color = sf::Color(50, 255, 50);
          break;
        default:
          break;
      }
      auto t = MakeText(symbol, x * tile_size_, y * tile_size_, color, 16);
      window_.draw(t);
    }
  }
}

void Renderer::DrawPlayer(Player& player) {
  auto t = MakeText("@", player.GetX() * tile_size_, player.GetY() * tile_size_,
                    sf::Color::Yellow, 16);
  window_.draw(t);
}

void Renderer::DrawEnemies(Room& room) {
  for (auto& enemy : room.GetEnemies()) {
    std::string symbol = "E";
    sf::Color color = sf::Color(255, 80, 80);
    if (enemy->GetEnemyId() == "infernal_demon") color = sf::Color(255, 0, 0);
    auto t = MakeText(symbol, enemy->GetX() * tile_size_,
                      enemy->GetY() * tile_size_, color, 16);
    window_.draw(t);
  }
}

void Renderer::DrawUI(GameState& game) {
  Player& p = game.GetPlayer();
  float ui_x = 620.f;

  window_.draw(MakeText("LIVING HELL", ui_x, 10.f, sf::Color::White, 16));
  window_.draw(MakeText("HP:   " + std::to_string(p.GetHealth()) + "/" +
                            std::to_string(p.GetMaxHealth()),
                        ui_x, 40.f, sf::Color(100, 255, 100), 14));
  window_.draw(MakeText("HEAT: " + std::to_string(p.GetCoreHeat()) + "/" +
                            std::to_string(p.GetMaxCoreHeat()),
                        ui_x, 60.f, sf::Color(255, 150, 50), 14));
  window_.draw(MakeText("LVL:  " + std::to_string(p.GetLevel()), ui_x, 80.f,
                        sf::Color::White, 14));
  window_.draw(MakeText("EXP:  " + std::to_string(p.GetExp()), ui_x, 100.f,
                        sf::Color::White, 14));
  window_.draw(MakeText("TIME: " + std::to_string(game.GetCurrentTimer()), ui_x,
                        120.f, sf::Color(100, 200, 255), 14));

  if (game.GetStatus() == GameStatus::kDefeat)
    window_.draw(MakeText("GAME OVER", ui_x, 200.f, sf::Color::Red, 20));
  else if (game.GetStatus() == GameStatus::kVictory)
    window_.draw(MakeText("VICTORY!", ui_x, 200.f, sf::Color::Green, 20));
}

sf::Text Renderer::MakeText(const std::string& str, float x, float y,
                            sf::Color color, int size) {
  sf::Text text(font_, str, size);
  text.setPosition({x, y});
  text.setFillColor(color);
  return text;
}
