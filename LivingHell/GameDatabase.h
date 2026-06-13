#pragma once

#include <string>
#include <vector>

#include "Enemy.h"
#include "Item.h"

struct EnemyData {
  std::string id;
  std::string name;
  int health = 0;
  int damage = 0;
  int armor = 0;
  int attack_range = 0;
  int move_speed = 0;
  int heat_damage = 0;
  int exp_reward = 0;
  int circle = 0;
};

struct ItemData {
  std::string id;
  std::string name;
  std::string type;
  int slots_required = 0;
  float effect_value = 0.f;
  float effect_value2 = 0.f;
  std::string effect_type;
};

struct RoomData {
  std::string room_type;
  int width = 0;
  int height = 0;
  int min_enemies = 0;
  int max_enemies = 0;
  bool has_chest = false;
  bool has_key = false;
  bool has_soul_ashurn = false;
  float spawn_weight_c1 = 0.f;
  float spawn_weight_c2 = 0.f;
  float spawn_weight_c3 = 0.f;
};

struct PlayerLevelData {
  int level;
  int max_health;
  int max_core_heat;
  int attack_damage_bonus;
  int attack_range_bonus;
  int exp_required;  // exp needed (cumulative) to level up from this level
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

  void LoadRooms(const std::string& path);
  const RoomData* GetRoomData(const std::string& room_type) const;
  const std::vector<RoomData>& GetAllRooms() const;

  void LoadPlayerStats(const std::string& path);
  const PlayerLevelData* GetPlayerLevelData(int level) const;
  int GetExpRequired(int level) const;  // exp needed to level up from level

 private:
  std::vector<EnemyData> enemies_;
  std::vector<ItemData> items_;

 
  std::vector<std::string> SplitCsv(const std::string& line) const;

  std::vector<RoomData> rooms_;

  std::vector<PlayerLevelData> player_stats_;
};
