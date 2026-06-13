#include "Renderer.h"

#include <cmath>

#include "DroppableChest.h"
#include "Inventory.h"
#include "Item.h"

Renderer::Renderer(sf::RenderWindow& window, const std::string& font_path)
    : window_(window), tile_size_(20.f) {
  font_.openFromFile(font_path);
}

void Renderer::Draw(GameState& game) {
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
        case TileType::kExit: {
          symbol = ">";
          float t = clock_.getElapsedTime().asSeconds();
          int g = 150 + static_cast<int>(105 * std::sin(t * 3.0f));
          color = sf::Color(0, g, 0);
          break;
        }
        case TileType::kKey: {
          symbol = "K";
          float t = clock_.getElapsedTime().asSeconds();
          int rb = 100 + static_cast<int>(80 * std::sin(t * 4.0f));
          color = sf::Color(255, 220, rb);
          break;
        }
        case TileType::kMagma: {
          symbol = "^";
          float t = clock_.getElapsedTime().asSeconds();
          int g = 40 + static_cast<int>(40 * std::sin(t * 3.0f));
          color = sf::Color(220, g, 0);
          break;
        }
        case TileType::kLava: {
          symbol = "~";
          float t = clock_.getElapsedTime().asSeconds();
          int g = static_cast<int>(30 * std::sin(t * 5.0f));
          color = sf::Color(255, 60 + g, 0);
          break;
        }
        case TileType::kAsh: {
          symbol = "%";
          color = sf::Color(120, 100, 80);
          break;
        }
        case TileType::kIce: {
          symbol = "*";
          float t = clock_.getElapsedTime().asSeconds();
          int b = 200 + static_cast<int>(55 * std::sin(t * 2.0f));
          color = sf::Color(100, 180, b);
          break;
        }
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
  float t = clock_.getElapsedTime().asSeconds();

  for (auto& enemy : room.GetEnemies()) {
    if (!enemy->IsAlive()) continue;
    std::string symbol;
    sf::Color color;

    if (enemy->GetEnemyId() == "chertila") {
      symbol = "e";
      int r = 200 + static_cast<int>(55 * std::sin(t * 2.0f));
      color = sf::Color(r, 80, 80);
    } else if (enemy->GetEnemyId() == "demon") {
      symbol = "D";
      int r = 200 + static_cast<int>(55 * std::sin(t * 2.5f));
      color = sf::Color(r, 0, 0);
    } else if (enemy->GetEnemyId() == "infernal_demon") {
      symbol = "I";
      int g = static_cast<int>(60 * (0.5f + 0.5f * std::sin(t * 4.0f)));
      color = sf::Color(255, g, 0);
    } else if (enemy->GetEnemyId() == "devil") {
      if (enemy->IsEnraged()) {
        // Фаза 2: белый → красный пульс, больший размер
        float pulse = 0.5f + 0.5f * std::sin(t * 8.0f);
        int r = 255;
        int g = static_cast<int>(255 * (1.0f - pulse));
        int b = static_cast<int>(255 * (1.0f - pulse));
        int sz = (pulse > 0.8f) ? 22 : 18;
        auto text = MakeText("B", enemy->GetX() * tile_size_,
                             enemy->GetY() * tile_size_, sf::Color(r, g, b), sz);
        window_.draw(text);
      } else {
        // Фаза 1: красный → жёлтый пульс
        int g = static_cast<int>(220 * (0.5f + 0.5f * std::sin(t * 5.0f)));
        int sz = (std::sin(t * 5.0f) > 0.7f) ? 20 : 16;
        auto text = MakeText("B", enemy->GetX() * tile_size_,
                             enemy->GetY() * tile_size_, sf::Color(255, g, 0), sz);
        window_.draw(text);
      }
      continue;
    } else {
      symbol = "E";
      color = sf::Color(200, 50, 50);
    }

    window_.draw(MakeText(symbol, enemy->GetX() * tile_size_,
                          enemy->GetY() * tile_size_, color, 16));
  }
}

void Renderer::DrawUI(GameState& game) {
  Player& p = game.GetPlayer();
  float ui_x = 620.f;
  float abl = 140.f;

  for (int i = 0; i < (int)p.GetAbilities().size(); i++) {
    auto& ab = p.GetAbilities()[i];
    sf::Color c = ab->IsReady() ? sf::Color::Cyan : sf::Color(100, 100, 100);

    std::string label = "[" + std::to_string(i + 1) + "] " + ab->GetName() +
                        (ab->IsReady() ? "" : " (cd)");
    window_.draw(MakeText(label, ui_x, abl, c, 13));
    abl += 18.f;
  }

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
    window_.draw(MakeText("GAME OVER", ui_x, abl + 8.f, sf::Color::Red, 18));
  else if (game.GetStatus() == GameStatus::kVictory) {
    window_.draw(MakeText("CORE DESTROYED", ui_x, abl + 8.f, sf::Color(255, 80, 0), 18));
    window_.draw(MakeText("The Hell is collapsing...", ui_x, abl + 30.f, sf::Color(200, 60, 0), 13));
  }

  if (p.HasKey()) {
    window_.draw(MakeText("[K] Key: found", ui_x, 405.f,
                          sf::Color(255, 220, 50), 12));
  } else {
    window_.draw(MakeText("[K] Key: not found", ui_x, 405.f,
                          sf::Color(100, 100, 100), 12));
  }

  const Inventory& inv = p.GetInventory();
  float inv_y = 435.f;
  window_.draw(MakeText(
      "INVENTORY [" + std::to_string(inv.GetUsedSlots()) + "/" +
          std::to_string(inv.GetMaxSlots()) + "]",
      ui_x, inv_y, sf::Color(200, 200, 200), 13));
  inv_y += 16.f;
  if (inv.GetItemCount() == 0) {
    window_.draw(MakeText("  (empty)", ui_x, inv_y, sf::Color(100, 100, 100), 12));
    inv_y += 14.f;
  } else {
    for (int i = 0; i < inv.GetItemCount(); i++) {
      Item* item = inv.GetItem(i);
      if (!item) continue;
      bool active = (i == inv.GetActiveIndex());
      sf::Color c = active ? sf::Color(255, 220, 50) : sf::Color(180, 180, 180);
      std::string prefix = active ? "> " : "  ";
      window_.draw(MakeText(prefix + item->GetName(), ui_x, inv_y, c, 12));
      inv_y += 14.f;
    }
  }
  window_.draw(MakeText("[X] use  [Tab] switch", ui_x, inv_y + 4.f,
                        sf::Color(80, 80, 80), 11));
}

sf::Text Renderer::MakeText(const std::string& str, float x, float y,
                            sf::Color color, int size) {
  sf::Text text(font_, str, size);
  text.setPosition({x, y});
  text.setFillColor(color);
  return text;
}

void Renderer::DrawChests(Room& room) {
  for (auto& obj : room.GetDestructibles()) {
    auto* chest = dynamic_cast<DroppableChest*>(obj.get());
    if (chest && chest->IsAlive()) {
      auto t =
          MakeText("C", chest->GetX() * tile_size_, chest->GetY() * tile_size_,
                   sf::Color(200, 150, 50), 16);
      window_.draw(t);
    }
  }
}

void Renderer::DrawFloorItems(Room& room) {
  float t = clock_.getElapsedTime().asSeconds();
  int b = 180 + static_cast<int>(75 * std::sin(t * 4.0f));
  for (const auto& entry : room.GetFloorItems()) {
    window_.draw(MakeText("i", entry.x * tile_size_, entry.y * tile_size_,
                          sf::Color(30, 180, b), 16));
  }
}

void Renderer::DrawTransition(int alpha, int circle) {
  sf::RectangleShape overlay(
      sf::Vector2f(static_cast<float>(window_.getSize().x),
                   static_cast<float>(window_.getSize().y)));
  overlay.setFillColor(sf::Color(0, 0, 0, static_cast<uint8_t>(alpha)));
  window_.draw(overlay);

  if (alpha > 180) {
    std::string label = "ENTERING CIRCLE " + std::to_string(circle);
    auto text = MakeText(label, 0.f, 0.f, sf::Color(255, 80, 0, static_cast<uint8_t>(alpha)), 22);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition({(window_.getSize().x - bounds.size.x) / 2.f,
                      (window_.getSize().y - bounds.size.y) / 2.f - 10.f});
    window_.draw(text);
  }
}

void Renderer::DrawDestructibles(Room& room) {
  for (auto& d : room.GetDestructibles()) {
    if (!d->IsAlive()) continue;
    if (dynamic_cast<DroppableChest*>(d.get())) continue;
    auto t = MakeText("*", d->GetX() * tile_size_, d->GetY() * tile_size_,
                      sf::Color(150, 100, 255), 16);
    window_.draw(t);
  }
}
