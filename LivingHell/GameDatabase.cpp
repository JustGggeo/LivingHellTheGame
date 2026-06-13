#include "GameDatabase.h"

#include <fstream>
#include <sstream>

void GameDatabase::LoadEnemies(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) return;
  std::string line;
  std::getline(file, line);  // пропускаем заголовок
  while (std::getline(file, line)) {
    auto fields = SplitCsv(line);
    if (fields.size() < 10) continue;
    EnemyData data;
    data.id = fields[0];
    data.name = fields[1];
    data.health = std::stoi(fields[2]);
    data.damage = std::stoi(fields[3]);
    data.armor = std::stoi(fields[4]);
    data.attack_range = std::stoi(fields[5]);
    data.move_speed = std::stoi(fields[6]);
    data.heat_damage = std::stoi(fields[7]);
    data.exp_reward = std::stoi(fields[8]);
    data.circle = std::stoi(fields[9]);
    enemies_.push_back(data);
  }
}

void GameDatabase::LoadItems(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) return;
  std::string line;
  std::getline(file, line);
  while (std::getline(file, line)) {
    auto fields = SplitCsv(line);
    if (fields.size() < 7) continue;
    ItemData data;
    data.id = fields[0];
    data.name = fields[1];
    data.type = fields[2];
    data.slots_required = std::stoi(fields[3]);
    data.effect_value = std::stof(fields[4]);
    data.effect_value2 = std::stof(fields[5]);
    data.effect_type = fields[6];
    items_.push_back(data);
  }
}

const EnemyData* GameDatabase::GetEnemyData(const std::string& id) const {
  for (const auto& e : enemies_)
    if (e.id == id) return &e;
  return nullptr;
}

const ItemData* GameDatabase::GetItemData(const std::string& id) const {
  for (const auto& i : items_)
    if (i.id == id) return &i;
  return nullptr;
}

const std::vector<EnemyData>& GameDatabase::GetAllEnemies() const {
  return enemies_;
}

const std::vector<ItemData>& GameDatabase::GetAllItems() const {
  return items_;
}

std::vector<std::string> GameDatabase::SplitCsv(const std::string& line) const {
  std::vector<std::string> result;
  std::stringstream ss(line);
  std::string field;
  while (std::getline(ss, field, ',')) result.push_back(field);
  return result;
}

void GameDatabase::LoadRooms(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) return;
  std::string line;
  std::getline(file, line);
  while (std::getline(file, line)) {
    auto fields = SplitCsv(line);
    if (fields.size() < 11) continue;
    RoomData data;
    data.room_type = fields[0];
    data.width = std::stoi(fields[1]);
    data.height = std::stoi(fields[2]);
    data.min_enemies = std::stoi(fields[3]);
    data.max_enemies = std::stoi(fields[4]);
    data.has_chest = std::stoi(fields[5]);
    data.has_key = std::stoi(fields[6]);
    data.has_soul_ashurn = std::stoi(fields[7]);
    data.spawn_weight_c1 = std::stof(fields[8]);
    data.spawn_weight_c2 = std::stof(fields[9]);
    data.spawn_weight_c3 = std::stof(fields[10]);
    rooms_.push_back(data);
  }
}

const RoomData* GameDatabase::GetRoomData(const std::string& room_type) const {
  for (const auto& r : rooms_)
    if (r.room_type == room_type) return &r;
  return nullptr;
}

const std::vector<RoomData>& GameDatabase::GetAllRooms() const {
  return rooms_;
}

void GameDatabase::LoadPlayerStats(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) return;
  std::string line;
  std::getline(file, line);  // пропускаем заголовок
  while (std::getline(file, line)) {
    auto fields = SplitCsv(line);
    if (fields.size() < 6) continue;
    PlayerLevelData data;
    data.level = std::stoi(fields[0]);
    data.max_health = std::stoi(fields[1]);
    data.max_core_heat = std::stoi(fields[2]);
    data.attack_damage_bonus = std::stoi(fields[3]);
    data.attack_range_bonus = std::stoi(fields[4]);
    data.exp_required = std::stoi(fields[5]);
    player_stats_.push_back(data);
  }
}

const PlayerLevelData* GameDatabase::GetPlayerLevelData(int level) const {
  for (const auto& d : player_stats_)
    if (d.level == level) return &d;
  return nullptr;
}

int GameDatabase::GetExpRequired(int level) const {
  const PlayerLevelData* data = GetPlayerLevelData(level);
  if (!data) return 0;
  return data->exp_required;
}
