#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Destructible.h"
#include "Enemy.h"
#include "Item.h"
#include "Tile.h"

enum class RoomType { kCombat, kResource, kTerminal, kAnomaly };

class Player;

class Room {
 public:
  struct FloorItem {
    int x, y;
    std::unique_ptr<Item> item;
  };

  Room(int width, int height, RoomType type);

  void Enter(Player& player);
  bool IsCleared() const;

  Tile& GetTile(int x, int y);
  const Tile& GetTile(int x, int y) const;
  RoomType GetRoomType() const;

  void AddEnemy(std::unique_ptr<Enemy> enemy);
  void AddDestructible(std::unique_ptr<Destructible> obj);
  void RemoveDeadEnemies(Player& player);

  std::vector<std::unique_ptr<Enemy>>& GetEnemies();
  std::vector<std::unique_ptr<Destructible>>& GetDestructibles();

  void AddDroppedItem(int x, int y, std::unique_ptr<Item> item);
  std::unique_ptr<Item> PickUpItemAt(int x, int y);
  bool HasItemAt(int x, int y) const;
  const std::vector<FloorItem>& GetFloorItems() const;

  int GetWidth() const;
  int GetHeight() const;

 private:
  int width_;
  int height_;
  RoomType type_;
  bool is_cleared_;
  std::vector<Tile> tiles_;
  std::vector<std::unique_ptr<Enemy>> enemies_;
  std::vector<std::unique_ptr<Destructible>> destructibles_;
  std::vector<FloorItem> floor_items_;

  void GenerateBasicLayout();
};