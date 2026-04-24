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
    data.circle = fields[9];
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
    if (fields.size() < 6) continue;
    ItemData data;
    data.id = fields[0];
    data.name = fields[1];
    data.type = fields[2];
    data.slots_required = std::stoi(fields[3]);
    data.effect_value = std::stof(fields[4]);
    data.effect_type = fields[5];
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
