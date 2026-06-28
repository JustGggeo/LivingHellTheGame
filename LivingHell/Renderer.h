#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <unordered_map>

#include "GameState.h"

class Renderer {
 public:
  Renderer(SDL_Renderer* renderer, const std::string& font_path);
  ~Renderer();

  void Draw(GameState& game);
  void DrawTransition(int alpha, int circle);

 private:
  static constexpr float kBaseWidth = 800.f;
  static constexpr float kBaseHeight = 600.f;

  SDL_Renderer* renderer_;
  TTF_Font* font_ = nullptr;
  Uint32 start_ticks_;
  float tile_size_;
  float scale_ = 1.f;
  float offset_x_ = 0.f;
  float offset_y_ = 0.f;
  std::unordered_map<std::string, SDL_Texture*> glyph_cache_;

  float GetElapsedSeconds() const;
  void UpdateScale();
  void DrawRoom(Room& room);
  void DrawPlayer(Player& player);
  void DrawEnemies(Room& room);
  void DrawDestructibles(Room& room);
  void DrawChests(Room& room);
  void DrawUI(GameState& game);
  void DrawFloorItems(Room& room);

  SDL_Texture* GetGlyphTexture(const std::string& str, int point_size,
                               int& out_w, int& out_h);
  void DrawText(const std::string& str, float x, float y, SDL_Color color,
               int size);
};
