#include "Tile.h"

Tile::Tile() : x_(0), y_(0), type_(TileType::kFloor) {}

Tile::Tile(int x, int y, TileType type) : x_(x), y_(y), type_(type) {}

TileType Tile::GetType() const { return type_; }
void Tile::SetType(TileType type) { type_ = type; }
int Tile::GetX() const { return x_; }
int Tile::GetY() const { return y_; }

bool Tile::IsWalkable() const {
  return type_ == TileType::kFloor || type_ == TileType::kDoor ||
         type_ == TileType::kExit || type_ == TileType::kKey ||
         type_ == TileType::kMagma || type_ == TileType::kLava ||
         type_ == TileType::kAsh || type_ == TileType::kIce;
}
