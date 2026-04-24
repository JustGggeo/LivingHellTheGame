#include "Room.h"

#include "Player.h"

Room::Room(int width, int height, RoomType type)
    : width_(width),
      height_(height),
      type_(type),
      is_cleared_(false),
      tiles_(width * height) {
  GenerateBasicLayout();
}

void Room::GenerateBasicLayout() {
  for (int y = 0; y < height_; y++) {
    for (int x = 0; x < width_; x++) {
      TileType t = (x == 0 || x == width_ - 1 || y == 0 || y == height_ - 1)
                       ? TileType::kWall
                       : TileType::kFloor;
      tiles_[y * width_ + x] = Tile(x, y, t);
    }
  }
}

void Room::Enter(Player& player) { is_cleared_ = false; }

bool Room::IsCleared() const {
  return enemies_.empty() && !is_cleared_ == false;
}

Tile& Room::GetTile(int x, int y) { return tiles_[y * width_ + x]; }

const Tile& Room::GetTile(int x, int y) const { return tiles_[y * width_ + x]; }

void Room::AddEnemy(std::unique_ptr<Enemy> enemy) {
  enemies_.push_back(std::move(enemy));
}

void Room::AddDestructible(std::unique_ptr<Destructible> obj) {
  destructibles_.push_back(std::move(obj));
}

void Room::RemoveDeadEnemies(Player& player) {
  for (auto it = enemies_.begin(); it != enemies_.end();) {
    if (!(*it)->IsAlive()) {
      player.GainExp((*it)->GetExpReward());
      it = enemies_.erase(it);
    } else {
      ++it;
    }
  }
}

std::vector<std::unique_ptr<Enemy>>& Room::GetEnemies() { return enemies_; }

std::vector<std::unique_ptr<Destructible>>& Room::GetDestructibles() {
  return destructibles_;
}

RoomType Room::GetRoomType() const { return type_; }
int Room::GetWidth() const { return width_; }
int Room::GetHeight() const { return height_; }
