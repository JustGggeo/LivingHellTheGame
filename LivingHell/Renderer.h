#pragma once

#include <SFML/Graphics.hpp>

#include "GameState.h"

class Renderer {
 public:
  Renderer(sf::RenderWindow& window, const std::string& font_path);

  void Draw(GameState& game);

 private:
  void DrawChests(Room& room);
  void DrawFloorItems(Room& room);

  sf::RenderWindow& window_;
  sf::Font font_;
  float tile_size_;

  void DrawRoom(Room& room);
  void DrawPlayer(Player& player);
  void DrawEnemies(Room& room);
  void DrawUI(GameState& game);

  sf::Text MakeText(const std::string& str, float x, float y, sf::Color color,
                    int size);
};
