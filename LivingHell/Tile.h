#pragma once

enum class TileType { kFloor, kWall, kDoor, kExit };

class Tile {
 public:
  Tile();
  Tile(int x, int y, TileType type);

  TileType GetType() const;
  void SetType(TileType type);
  int GetX() const;
  int GetY() const;
  bool IsWalkable() const;

 private:
  int x_;
  int y_;
  TileType type_;
};
