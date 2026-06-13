#pragma once

#include <SFML/Graphics.hpp>

#include "GameState.h"

class Renderer {
 public:
  Renderer(sf::RenderWindow& window, const std::string& font_path);

  void Draw(GameState& game);
  void DrawTransition(int alpha, int circle);

 private:
  sf::RenderWindow& window_;
  sf::Font font_;
  sf::Clock clock_;
  float tile_size_;

  void DrawRoom(Room& room);
  void DrawPlayer(Player& player);
  void DrawEnemies(Room& room);
  void DrawDestructibles(Room& room);
  void DrawChests(Room& room);
  void DrawUI(GameState& game);
  void DrawFloorItems(Room& room);

  sf::Text MakeText(const std::string& str, float x, float y, sf::Color color,
                    int size);
};
