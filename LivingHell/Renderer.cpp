#include "Renderer.h"

#include <algorithm>
#include <cmath>

#include "DroppableChest.h"
#include "Inventory.h"
#include "Item.h"

Renderer::Renderer(SDL_Renderer* renderer, const std::string& font_path)
    : renderer_(renderer), tile_size_(20.f) {
  font_ = TTF_OpenFont(font_path.c_str(), 16);
  start_ticks_ = SDL_GetTicks();
}

Renderer::~Renderer() {
  for (auto& [key, tex] : glyph_cache_) SDL_DestroyTexture(tex);
  if (font_) TTF_CloseFont(font_);
}

float Renderer::GetElapsedSeconds() const {
  return (SDL_GetTicks() - start_ticks_) / 1000.f;
}

void Renderer::UpdateScale() {
  int w = 0, h = 0;
  SDL_GetRendererOutputSize(renderer_, &w, &h);
  scale_ = std::min(static_cast<float>(w) / kBaseWidth,
                    static_cast<float>(h) / kBaseHeight);
  offset_x_ = (static_cast<float>(w) - kBaseWidth * scale_) / 2.f;
  offset_y_ = (static_cast<float>(h) - kBaseHeight * scale_) / 2.f;
}

void Renderer::Draw(GameState& game) {
  UpdateScale();
  DrawRoom(game.GetCurrentRoom());
  DrawChests(game.GetCurrentRoom());
  DrawFloorItems(game.GetCurrentRoom());
  DrawDestructibles(game.GetCurrentRoom());
  DrawPlayer(game.GetPlayer());
  DrawEnemies(game.GetCurrentRoom());
  DrawUI(game);
}

void Renderer::DrawRoom(Room& room) {
  for (int y = 0; y < room.GetHeight(); y++) {
    for (int x = 0; x < room.GetWidth(); x++) {
      Tile& tile = room.GetTile(x, y);
      std::string symbol = ".";
      SDL_Color color = {80, 80, 80, 255};
      switch (tile.GetType()) {
        case TileType::kWall:
          symbol = "#";
          color = {180, 180, 180, 255};
          break;
        case TileType::kDoor:
          symbol = "+";
          color = {200, 150, 50, 255};
          break;
        case TileType::kExit: {
          symbol = ">";
          float t = GetElapsedSeconds();
          Uint8 g = static_cast<Uint8>(150 + 105 * std::sin(t * 3.0f));
          color = {0, g, 0, 255};
          break;
        }
        case TileType::kKey: {
          symbol = "K";
          float t = GetElapsedSeconds();
          Uint8 rb = static_cast<Uint8>(100 + 80 * std::sin(t * 4.0f));
          color = {255, 220, rb, 255};
          break;
        }
        case TileType::kMagma: {
          symbol = "^";
          float t = GetElapsedSeconds();
          Uint8 g = static_cast<Uint8>(40 + 40 * std::sin(t * 3.0f));
          color = {220, g, 0, 255};
          break;
        }
        case TileType::kLava: {
          symbol = "~";
          float t = GetElapsedSeconds();
          Uint8 g = static_cast<Uint8>(60 + 30 * std::sin(t * 5.0f));
          color = {255, g, 0, 255};
          break;
        }
        case TileType::kAsh: {
          symbol = "%";
          color = {120, 100, 80, 255};
          break;
        }
        case TileType::kIce: {
          symbol = "*";
          float t = GetElapsedSeconds();
          Uint8 b = static_cast<Uint8>(200 + 55 * std::sin(t * 2.0f));
          color = {100, 180, b, 255};
          break;
        }
        default:
          break;
      }
      DrawText(symbol, x * tile_size_, y * tile_size_, color, 16);
    }
  }
}

void Renderer::DrawPlayer(Player& player) {
  DrawText("@", player.GetX() * tile_size_, player.GetY() * tile_size_,
           SDL_Color{255, 255, 0, 255}, 16);
}

void Renderer::DrawEnemies(Room& room) {
  float t = GetElapsedSeconds();

  for (auto& enemy : room.GetEnemies()) {
    if (!enemy->IsAlive()) continue;
    std::string symbol;
    SDL_Color color;

    if (enemy->GetEnemyId() == "chertila") {
      symbol = "e";
      Uint8 r = static_cast<Uint8>(200 + 55 * std::sin(t * 2.0f));
      color = {r, 80, 80, 255};
    } else if (enemy->GetEnemyId() == "demon") {
      symbol = "D";
      Uint8 r = static_cast<Uint8>(200 + 55 * std::sin(t * 2.5f));
      color = {r, 0, 0, 255};
    } else if (enemy->GetEnemyId() == "infernal_demon") {
      symbol = "I";
      Uint8 g = static_cast<Uint8>(60 * (0.5f + 0.5f * std::sin(t * 4.0f)));
      color = {255, g, 0, 255};
    } else if (enemy->GetEnemyId() == "devil") {
      if (enemy->IsEnraged()) {
        // Фаза 2: белый → красный пульс, больший размер
        float pulse = 0.5f + 0.5f * std::sin(t * 8.0f);
        Uint8 r = 255;
        Uint8 g = static_cast<Uint8>(255 * (1.0f - pulse));
        Uint8 b = static_cast<Uint8>(255 * (1.0f - pulse));
        int sz = (pulse > 0.8f) ? 22 : 18;
        DrawText("B", enemy->GetX() * tile_size_, enemy->GetY() * tile_size_,
                 SDL_Color{r, g, b, 255}, sz);
      } else {
        // Фаза 1: красный → жёлтый пульс
        Uint8 g = static_cast<Uint8>(220 * (0.5f + 0.5f * std::sin(t * 5.0f)));
        int sz = (std::sin(t * 5.0f) > 0.7f) ? 20 : 16;
        DrawText("B", enemy->GetX() * tile_size_, enemy->GetY() * tile_size_,
                 SDL_Color{255, g, 0, 255}, sz);
      }
      continue;
    } else {
      symbol = "E";
      color = {200, 50, 50, 255};
    }

    DrawText(symbol, enemy->GetX() * tile_size_, enemy->GetY() * tile_size_,
             color, 16);
  }
}

void Renderer::DrawUI(GameState& game) {
  Player& p = game.GetPlayer();
  float ui_x = 620.f;
  float abl = 140.f;

  for (int i = 0; i < (int)p.GetAbilities().size(); i++) {
    auto& ab = p.GetAbilities()[i];
    SDL_Color c = ab->IsReady() ? SDL_Color{0, 255, 255, 255}
                                : SDL_Color{100, 100, 100, 255};

    std::string label = "[" + std::to_string(i + 1) + "] " + ab->GetName() +
                        (ab->IsReady() ? "" : " (cd)");
    DrawText(label, ui_x, abl, c, 13);
    abl += 18.f;
  }

  DrawText("LIVING HELL", ui_x, 10.f, SDL_Color{255, 255, 255, 255}, 16);
  DrawText("HP:   " + std::to_string(p.GetHealth()) + "/" +
              std::to_string(p.GetMaxHealth()),
          ui_x, 40.f, SDL_Color{100, 255, 100, 255}, 14);
  DrawText("HEAT: " + std::to_string(p.GetCoreHeat()) + "/" +
              std::to_string(p.GetMaxCoreHeat()),
          ui_x, 60.f, SDL_Color{255, 150, 50, 255}, 14);
  DrawText("LVL:  " + std::to_string(p.GetLevel()), ui_x, 80.f,
          SDL_Color{255, 255, 255, 255}, 14);
  DrawText("EXP:  " + std::to_string(p.GetExp()), ui_x, 100.f,
          SDL_Color{255, 255, 255, 255}, 14);
  DrawText("TIME: " + std::to_string(game.GetCurrentTimer()), ui_x, 120.f,
          SDL_Color{100, 200, 255, 255}, 14);

  if (game.GetStatus() == GameStatus::kDefeat) {
    DrawText("GAME OVER", ui_x, abl + 8.f, SDL_Color{255, 0, 0, 255}, 18);
    DrawText("[R] Restart", ui_x, abl + 30.f, SDL_Color{150, 80, 80, 255}, 13);
  } else if (game.GetStatus() == GameStatus::kVictory) {
    DrawText("CORE DESTROYED", ui_x, abl + 8.f, SDL_Color{255, 80, 0, 255},
            18);
    DrawText("The Hell is collapsing...", ui_x, abl + 30.f,
            SDL_Color{200, 60, 0, 255}, 13);
    DrawText("[R] Restart", ui_x, abl + 48.f, SDL_Color{150, 80, 80, 255}, 13);
  }

  if (p.HasKey()) {
    DrawText("[K] Key: found", ui_x, 405.f, SDL_Color{255, 220, 50, 255}, 12);
  } else {
    DrawText("[K] Key: not found", ui_x, 405.f,
            SDL_Color{100, 100, 100, 255}, 12);
  }

  const Inventory& inv = p.GetInventory();
  float inv_y = 435.f;
  DrawText("INVENTORY [" + std::to_string(inv.GetUsedSlots()) + "/" +
              std::to_string(inv.GetMaxSlots()) + "]",
          ui_x, inv_y, SDL_Color{200, 200, 200, 255}, 13);
  inv_y += 16.f;
  if (inv.GetItemCount() == 0) {
    DrawText("  (empty)", ui_x, inv_y, SDL_Color{100, 100, 100, 255}, 12);
    inv_y += 14.f;
  } else {
    for (int i = 0; i < inv.GetItemCount(); i++) {
      Item* item = inv.GetItem(i);
      if (!item) continue;
      bool active = (i == inv.GetActiveIndex());
      SDL_Color c = active ? SDL_Color{255, 220, 50, 255}
                           : SDL_Color{180, 180, 180, 255};
      std::string prefix = active ? "> " : "  ";
      DrawText(prefix + item->GetName(), ui_x, inv_y, c, 12);
      inv_y += 14.f;
    }
  }
  DrawText("[X] use  [Tab] switch", ui_x, inv_y + 4.f,
          SDL_Color{80, 80, 80, 255}, 11);
}

SDL_Texture* Renderer::GetGlyphTexture(const std::string& str, int point_size,
                                       int& out_w, int& out_h) {
  std::string key = str + "#" + std::to_string(point_size);
  auto it = glyph_cache_.find(key);
  if (it != glyph_cache_.end()) {
    SDL_QueryTexture(it->second, nullptr, nullptr, &out_w, &out_h);
    return it->second;
  }
  if (!font_) return nullptr;
  TTF_SetFontSize(font_, point_size);
  // Глиф рендерится один раз белым и кэшируется; цвет применяется через
  // SDL_SetTextureColorMod при каждой отрисовке — без этого каждый кадр
  // заново растеризовался бы шрифт для всех ~900 тайлов поля.
  SDL_Surface* surf =
      TTF_RenderText_Blended(font_, str.c_str(), SDL_Color{255, 255, 255, 255});
  if (!surf) return nullptr;
  SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
  out_w = surf->w;
  out_h = surf->h;
  SDL_FreeSurface(surf);
  if (!tex) return nullptr;
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  glyph_cache_[key] = tex;
  return tex;
}

void Renderer::DrawText(const std::string& str, float x, float y,
                        SDL_Color color, int size) {
  if (str.empty()) return;
  int scaled_size =
      std::max(1, static_cast<int>(std::round(size * scale_)));
  int w = 0, h = 0;
  SDL_Texture* tex = GetGlyphTexture(str, scaled_size, w, h);
  if (!tex) return;

  SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
  SDL_SetTextureAlphaMod(tex, color.a);
  SDL_Rect dst{static_cast<int>(offset_x_ + x * scale_),
              static_cast<int>(offset_y_ + y * scale_), w, h};
  SDL_RenderCopy(renderer_, tex, nullptr, &dst);
}

void Renderer::DrawChests(Room& room) {
  for (auto& obj : room.GetDestructibles()) {
    auto* chest = dynamic_cast<DroppableChest*>(obj.get());
    if (chest && chest->IsAlive()) {
      DrawText("C", chest->GetX() * tile_size_, chest->GetY() * tile_size_,
               SDL_Color{200, 150, 50, 255}, 16);
    }
  }
}

void Renderer::DrawFloorItems(Room& room) {
  float t = GetElapsedSeconds();
  Uint8 b = static_cast<Uint8>(180 + 75 * std::sin(t * 4.0f));
  for (const auto& entry : room.GetFloorItems()) {
    DrawText("i", entry.x * tile_size_, entry.y * tile_size_,
             SDL_Color{30, 180, b, 255}, 16);
  }
}

void Renderer::DrawTransition(int alpha, int circle) {
  int w = 0, h = 0;
  SDL_GetRendererOutputSize(renderer_, &w, &h);

  SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, static_cast<Uint8>(alpha));
  SDL_Rect overlay{0, 0, w, h};
  SDL_RenderFillRect(renderer_, &overlay);

  if (alpha > 180) {
    std::string label = "ENTERING CIRCLE " + std::to_string(circle);
    int scaled_size = std::max(1, static_cast<int>(std::round(22 * scale_)));
    int tw = 0, th = 0;
    SDL_Texture* tex = GetGlyphTexture(label, scaled_size, tw, th);
    if (tex) {
      SDL_SetTextureColorMod(tex, 255, 80, 0);
      SDL_SetTextureAlphaMod(tex, static_cast<Uint8>(alpha));
      SDL_Rect dst{(w - tw) / 2, (h - th) / 2 - 10, tw, th};
      SDL_RenderCopy(renderer_, tex, nullptr, &dst);
    }
  }
}

void Renderer::DrawDestructibles(Room& room) {
  for (auto& d : room.GetDestructibles()) {
    if (!d->IsAlive()) continue;
    if (dynamic_cast<DroppableChest*>(d.get())) continue;
    DrawText("*", d->GetX() * tile_size_, d->GetY() * tile_size_,
             SDL_Color{150, 100, 255, 255}, 16);
  }
}
