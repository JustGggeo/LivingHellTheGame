#pragma once

#include <memory>
#include <vector>

#include "Ability.h"
#include "Entity.h"
#include "Inventory.h"

class Player : public Entity {
 public:
  Player(int x, int y);

  Inventory& GetInventory();

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

  void UpdateStatsForLevel();
  void UnlockAbility(std::unique_ptr<Ability> ability);
  bool UseAbility(int index, Room& room);
  void TickAbilities();  // גûחûגאעü ךאזהûי ץמה
  const std::vector<std::unique_ptr<Ability>>& GetAbilities() const;

  bool HasEmissionBuff() const;
  void SetEmissionBuff(bool val);
  void ClearEmissionBuff();
  int GetAttackDamageBonus() const;
  int GetAttackRangeBonus() const;
  void AddAttackDamageBonus(int val);
  void AddAttackRangeBonus(int val);

 private:
  bool emission_buff_ = false;
  int attack_damage_bonus_ = 0;  // מע ןנוהלועמג
  int attack_range_bonus_ = 0;   // מע ןנוהלועמג

  int core_heat_;
  int max_core_heat_;
  int level_;
  int exp_;
  int attack_range_;
  Inventory inventory_;
  std::vector<std::unique_ptr<Ability>> abilities_;
};
