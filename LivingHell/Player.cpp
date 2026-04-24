#include "Player.h"
#include "HeatPurge.h"
#include "Abilities.h"

Player::Player(int x, int y) 
    : Entity(x, y, 5),
      core_heat_(0),
      max_core_heat_(3),
      level_(1),
      exp_(0),
      attack_range_(1),
      inventory_(12) 
{
  
    UnlockAbility(std::make_unique<BasicAttack>());
  
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
  }
}


void Player::UnlockAbility(std::unique_ptr<Ability> ability) {
  abilities_.push_back(std::move(ability));
}

bool Player::UseAbility(int index, Room& room) {
  if (index < 0 || index >= (int)abilities_.size()) return false;
  if (!abilities_[index]->IsReady()) return false;
  abilities_[index]->Activate(*this, room);
  return true;
}

void Player::TickAbilities() {
  for (auto& a : abilities_) a->OnTurnPassed();
}

const std::vector<std::unique_ptr<Ability>>& Player::GetAbilities() const {
  return abilities_;
}

bool Player::HasEmissionBuff() const { return emission_buff_;
}

void Player::SetEmissionBuff(bool val) { emission_buff_ = val;
}

void Player::ClearEmissionBuff() { emission_buff_ = false; }

int Player::GetAttackDamageBonus() const { return attack_damage_bonus_; 
}

int Player::GetAttackRangeBonus() const { return attack_range_bonus_;
}

void Player::AddAttackDamageBonus(int val) { attack_damage_bonus_ += val; 
}

void Player::AddAttackRangeBonus(int val) { attack_range_bonus_ += val;
}



void Player::TakeDamage(int dmg) {
  Entity::TakeDamage(dmg);
  if (health_ <= 0) health_ = 0;
}

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
  if (level_ == 1 && exp_ >= 4)
    LevelUp();
  else if (level_ == 2 && exp_ >= 8)
    LevelUp();
  else if (level_ == 3 && exp_ >= 12)
    LevelUp();
}

void Player::LevelUp() {
  if (level_ >= 4) return;
  level_++;
  UpdateStatsForLevel();
  // выдаём способность за уровень
  if (level_ == 2) UnlockAbility(std::make_unique<HeatPurge>());
  // level_ == 3, 4 заделаю позже
}

void Player::UpdateStatsForLevel() {
  switch (level_) {
    case 2:
      max_health_ = 8;
      max_core_heat_ = 5;
      break;
    case 3:
      max_health_ = 10;
      max_core_heat_ = 7;
      break;
    case 4:
      max_health_ = 12;
      max_core_heat_ = 10;
      break;
    default:
      break;
  }
  health_ = max_health_;
}

bool Player::UseItem(int index) { return inventory_.UseItem(index, *this); }

int Player::GetCoreHeat() const { return core_heat_; }
int Player::GetMaxCoreHeat() const { return max_core_heat_; }
int Player::GetLevel() const { return level_; }
int Player::GetExp() const { return exp_; }
int Player::GetAttackRange() const { return attack_range_; }

Inventory& Player::GetInventory() { return inventory_; }

