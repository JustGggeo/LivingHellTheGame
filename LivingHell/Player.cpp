#include "Player.h"

#include <algorithm>

#include "Abilities.h"
#include "Constants.h"
#include "GameDatabase.h"
#include "HeatPurge.h"
#include "Weapon.h"

Player::Player(int x, int y, const GameDatabase& db)
    : Entity(x, y, Constants::kPlayerInitialHealth),
      db_(&db),
      core_heat_(0),
      max_core_heat_(Constants::kPlayerInitialMaxCoreHeat),
      level_(1),
      exp_(0),
      attack_range_(Constants::kPlayerInitialAttackRange),
      inventory_(Constants::kPlayerInitialInventorySize) {
  UnlockAbility(std::make_unique<BasicAttack>());
}

void Player::TakeDamage(int dmg) {
  Entity::TakeDamage(dmg);
}

void Player::LevelUp() {
  if (level_ >= 4) return;
  level_++;
  UpdateStatsForLevel();

  switch (level_) {
    case 2:
      UnlockAbility(std::make_unique<Emission>());
      break;
    case 3:
      UnlockAbility(std::make_unique<Phasing>());
      UnlockAbility(std::make_unique<Deformation>());
      break;
    case 4:
      UnlockAbility(std::make_unique<Disintegration>());
      break;
    default:
      break;
  }
}

void Player::UnlockAbility(std::unique_ptr<Ability> ability) {
  abilities_.push_back(std::move(ability));
}

bool Player::UseAbility(int index, Room& room) {
  if (index < 0 || index >= static_cast<int>(abilities_.size())) return false;
  if (!abilities_[index]->IsReady()) return false;
  abilities_[index]->Activate(*this, room);
  NotifyAbilityObservers(*abilities_[index], room);
  return true;
}

void Player::TickAbilities() {
  for (auto& a : abilities_) a->OnTurnPassed();
}

const std::vector<std::unique_ptr<Ability>>& Player::GetAbilities() const {
  return abilities_;
}

bool Player::HasEmissionBuff() const { return emission_buff_; }
void Player::SetEmissionBuff(bool val) { emission_buff_ = val; }
void Player::ClearEmissionBuff() { emission_buff_ = false; }
bool Player::HasKey() const { return has_key_; }
void Player::PickUpKey() { has_key_ = true; }
void Player::ConsumeKey() { has_key_ = false; }

int Player::GetAttackDamageBonus() const {
  int bonus = attack_damage_bonus_;
  const Item* active = inventory_.GetActiveItem();
  if (active && active->GetType() == ItemType::kWeapon)
    bonus += static_cast<const Weapon*>(active)->GetDamage();
  return bonus;
}

int Player::GetAttackRangeBonus() const {
  int bonus = attack_range_bonus_;
  const Item* active = inventory_.GetActiveItem();
  if (active && active->GetType() == ItemType::kWeapon)
    bonus += static_cast<const Weapon*>(active)->GetMaxRange();
  return bonus;
}
void Player::AddAttackDamageBonus(int val) { attack_damage_bonus_ += val; }
void Player::AddAttackRangeBonus(int val) { attack_range_bonus_ += val; }

bool Player::Move(int dx, int dy) {
  x_ += dx;
  y_ += dy;
  return true;
}

void Player::AddCoreHeat(int amount) {
  core_heat_ += amount;
  if (core_heat_ > max_core_heat_) core_heat_ = max_core_heat_;
  if (core_heat_ < 0) core_heat_ = 0;
}

void Player::GainExp(int amount) {
  exp_ += amount;
  if (!db_) return;
  const PlayerLevelData* data = db_->GetPlayerLevelData(level_);
  if (data && data->exp_required > 0 && exp_ >= data->exp_required)
    LevelUp();
}

void Player::UpdateStatsForLevel() {
  if (!db_) return;
  const PlayerLevelData* data = db_->GetPlayerLevelData(level_);
  if (!data) return;
  max_health_ = data->max_health;
  max_core_heat_ = data->max_core_heat;
  attack_damage_bonus_ = data->attack_damage_bonus;
  attack_range_bonus_ = data->attack_range_bonus;
  health_ = max_health_;
}

bool Player::UseItem(int index) { return inventory_.UseItem(index, *this); }

int Player::GetCoreHeat() const { return core_heat_; }
int Player::GetMaxCoreHeat() const { return max_core_heat_; }
int Player::GetLevel() const { return level_; }
int Player::GetExp() const { return exp_; }
int Player::GetAttackRange() const { return attack_range_; }
Inventory& Player::GetInventory() { return inventory_; }
const Inventory& Player::GetInventory() const { return inventory_; }

void Player::AddAbilityObserver(IAbilityObserver* observer) {
  ability_observers_.push_back(observer);
}

void Player::RemoveAbilityObserver(IAbilityObserver* observer) {
  ability_observers_.erase(
      std::remove(ability_observers_.begin(), ability_observers_.end(),
                  observer),
      ability_observers_.end());
}

void Player::NotifyAbilityObservers(const Ability& ability, Room& room) {
  for (auto* obs : ability_observers_)
    obs->OnAbilityActivated(ability, *this, room);
}
