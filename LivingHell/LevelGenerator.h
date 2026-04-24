#pragma once

#include <cstdlib>
#include <memory>
#include <vector>

#include "GameDatabase.h"
#include "Room.h"

struct RoomPlacement {
  int x;
  int y;
  int width;
  int height;
  std::string room_type;
};

class LevelGenerator {
 public:
  LevelGenerator(int field_width, int field_height);

  void Generate(int circle, const GameDatabase& db);
  void ApplyToField(std::vector<std::vector<TileType>>& field) const;

  const std::vector<RoomPlacement>& GetPlacements() const;
  const RoomPlacement* GetStartRoom() const;
  const RoomPlacement* GetExitRoom() const;

 private:
  int field_width_;
  int field_height_;
  std::vector<RoomPlacement> placements_;

  bool Overlaps(const RoomPlacement& a, const RoomPlacement& b) const;
  std::string PickRoomType(int circle, const GameDatabase& db, bool used_start,
                           bool used_exit, bool used_key) const;

  void ConnectRooms(std::vector<std::vector<TileType>>& field) const;
  void CarveCoridor(std::vector<std::vector<TileType>>& field, int x1, int y1,
                    int x2, int y2) const;
};
