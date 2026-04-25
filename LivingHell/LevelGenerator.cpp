#include "LevelGenerator.h"

LevelGenerator::LevelGenerator(int field_width, int field_height)
    : field_width_(field_width), field_height_(field_height) {}

void LevelGenerator::Generate(int circle, const GameDatabase& db) {
  placements_.clear();
  int max_attempts = 100;
  int target_rooms = 10;
  bool used_key = false;

  // Сначала гарантированно спавним start
  const RoomData* start_data = db.GetRoomData("start");
  if (start_data) {
    for (int attempt = 0; attempt < max_attempts; attempt++) {
      RoomPlacement p;
      p.room_type = "start";
      p.width = start_data->width;
      p.height = start_data->height;
      p.x = 2 + rand() % (field_width_ - p.width - 4);
      p.y = 2 + rand() % (field_height_ - p.height - 4);
      bool ok = true;
      for (const auto& e : placements_)
        if (Overlaps(p, e)) {
          ok = false;
          break;
        }
      if (ok) {
        placements_.push_back(p);
        break;
      }
    }
  }

  // Спавним обычные комнаты
  for (int i = 0; i < target_rooms - 2; i++) {
    std::string type = PickRoomType(circle, db, true, true, used_key);
    const RoomData* data = db.GetRoomData(type);
    if (!data) continue;
    for (int attempt = 0; attempt < max_attempts; attempt++) {
      RoomPlacement p;
      p.room_type = type;
      p.width = data->width;
      p.height = data->height;
      p.x = 2 + rand() % (field_width_ - p.width - 4);
      p.y = 2 + rand() % (field_height_ - p.height - 4);
      bool ok = true;
      for (const auto& e : placements_)
        if (Overlaps(p, e)) {
          ok = false;
          break;
        }
      if (ok) {
        placements_.push_back(p);
        if (type == "keyroom") used_key = true;
        break;
      }
    }
  }

  // Гарантированно спавним exit последней
  const RoomData* exit_data = db.GetRoomData("exit");
  if (exit_data) {
    for (int attempt = 0; attempt < max_attempts; attempt++) {
      RoomPlacement p;
      p.room_type = "exit";
      p.width = exit_data->width;
      p.height = exit_data->height;
      p.x = 2 + rand() % (field_width_ - p.width - 4);
      p.y = 2 + rand() % (field_height_ - p.height - 4);
      bool ok = true;
      for (const auto& e : placements_)
        if (Overlaps(p, e)) {
          ok = false;
          break;
        }
      if (ok) {
        placements_.push_back(p);
        break;
      }
    }
  }
}

bool LevelGenerator::Overlaps(const RoomPlacement& a,
                              const RoomPlacement& b) const {
  int margin = 0;
  return !(a.x + a.width + margin <= b.x || b.x + b.width + margin <= a.x ||
           a.y + a.height + margin <= b.y || b.y + b.height + margin <= a.y);
}

std::string LevelGenerator::PickRoomType(int circle, const GameDatabase& db,
                                         bool used_start, bool used_exit,
                                         bool used_key) const {
  if (!used_start) return "start";
  if (!used_exit) return "exit";
  if (!used_key) return "keyroom";

  const auto& rooms = db.GetAllRooms();
  float total = 0.f;
  for (const auto& r : rooms) {
    if (r.room_type == "start" || r.room_type == "exit" ||
        r.room_type == "keyroom")
      continue;
    float w = (circle == 1)   ? r.spawn_weight_c1
              : (circle == 2) ? r.spawn_weight_c2
                              : r.spawn_weight_c3;
    total += w;
  }

  float roll = (rand() % 1000) / 1000.f * total;
  float acc = 0.f;
  for (const auto& r : rooms) {
    if (r.room_type == "start" || r.room_type == "exit" ||
        r.room_type == "keyroom")
      continue;
    float w = (circle == 1)   ? r.spawn_weight_c1
              : (circle == 2) ? r.spawn_weight_c2
                              : r.spawn_weight_c3;
    acc += w;
    if (roll <= acc) return r.room_type;
  }
  return "combat";
}

const std::vector<RoomPlacement>& LevelGenerator::GetPlacements() const {
  return placements_;
}

const RoomPlacement* LevelGenerator::GetStartRoom() const {
  for (const auto& p : placements_)
    if (p.room_type == "start") return &p;
  return nullptr;
}

const RoomPlacement* LevelGenerator::GetExitRoom() const {
  for (const auto& p : placements_)
    if (p.room_type == "exit") return &p;
  return nullptr;
}
