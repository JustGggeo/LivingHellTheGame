#pragma once

#include <SFML/Graphics.hpp>

#include "GameState.h"

class Renderer {
 public:
  Renderer(sf::RenderWindow& window, const std::string& font_path);

  void Draw(GameState& game);
  void DrawTransition(int alpha, int circle);

 private:
  static constexpr float kBaseWidth = 800.f;
  static constexpr float kBaseHeight = 600.f;

  sf::RenderWindow& window_;
  sf::Font font_;
  sf::Clock clock_;
  float tile_size_;
  float scale_ = 1.f;
  float offset_x_ = 0.f;
  float offset_y_ = 0.f;

  void UpdateScale();
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
