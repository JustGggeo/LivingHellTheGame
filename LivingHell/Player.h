#pragma once

#include <memory>
#include <vector>

#include "Ability.h"
#include "Entity.h"
#include "GameDatabase.h"
#include "IAbilityObserver.h"
#include "Inventory.h"

class Player : public Entity {
 public:
  Player(int x, int y, const GameDatabase& db);

  Inventory& GetInventory();
  const Inventory& GetInventory() const;

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

  void UpdateStatsForLevel();
  void UnlockAbility(std::unique_ptr<Ability> ability);
  bool UseAbility(int index, Room& room);
  void TickAbilities();
  const std::vector<std::unique_ptr<Ability>>& GetAbilities() const;

  bool HasKey() const;
  void PickUpKey();
  void ConsumeKey();

  bool HasEmissionBuff() const;
  void SetEmissionBuff(bool val);
  void ClearEmissionBuff();
  int GetAttackDamageBonus() const;
  int GetAttackRangeBonus() const;
  void AddAttackDamageBonus(int val);
  void AddAttackRangeBonus(int val);

  // --- Observer pattern ---
  void AddAbilityObserver(IAbilityObserver* observer);
  void RemoveAbilityObserver(IAbilityObserver* observer);

 private:
  const GameDatabase* db_ = nullptr;
  bool emission_buff_ = false;
  bool has_key_ = false;
  int attack_damage_bonus_ = 0;
  int attack_range_bonus_ = 0;
  int core_heat_;
  int max_core_heat_;
  int level_;
  int exp_;
  int attack_range_;
  Inventory inventory_;
  std::vector<std::unique_ptr<Ability>> abilities_;

  // --- Observer support ---
  std::vector<IAbilityObserver*> ability_observers_;
  void NotifyAbilityObservers(const Ability& ability, Room& room);
};
