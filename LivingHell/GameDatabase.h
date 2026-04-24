#pragma once

#include <string>
#include <vector>

#include "Enemy.h"
#include "Item.h"

struct EnemyData {
  std::string id;
  std::string name;
  int health;
  int damage;
  int armor;
  int attack_range;
  int move_speed;
  int heat_damage;
  int exp_reward;
  std::string circle;
};

struct ItemData {
  std::string id;
  std::string name;
  std::string type;
  int slots_required;
  float effect_value;
  std::string effect_type;
};

class GameDatabase {
 public:
  GameDatabase() = default;

  void LoadEnemies(const std::string& path);
  void LoadItems(const std::string& path);

  const EnemyData* GetEnemyData(const std::string& id) const;
  const ItemData* GetItemData(const std::string& id) const;

  const std::vector<EnemyData>& GetAllEnemies() const;
  const std::vector<ItemData>& GetAllItems() const;

 private:
  std::vector<EnemyData> enemies_;
  std::vector<ItemData> items_;

  std::vector<std::string> SplitCsv(const std::string& line) const;
};
