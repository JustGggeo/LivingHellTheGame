#pragma once

#include <vector>

#include "Entity.h"
#include "Inventory.h"

class Player : public Entity {
 public:
  Player(int x, int y);

  void TakeDamage(int dmg) override;
  bool Move(int dx, int dy) override;

  void GainExp(int amount);
  void LevelUp();
  void AddCoreHeat(int amount);
  bool UseItem(int index);

  int GetCoreHeat() const;
  int GetMaxCoreHeat() const;
  int GetLevel() const;
  int GetExp() const;
  int GetAttackRange() const;

  Inventory& GetInventory();

 private:
  int core_heat_;
  int max_core_heat_;
  int level_;
  int exp_;
  int attack_range_;
  Inventory inventory_;

  void UpdateStatsForLevel();
};
