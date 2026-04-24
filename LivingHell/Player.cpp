#include "Player.h"

Player::Player(int x, int y)
    : Entity(x, y, 5),
      core_heat_(0),
      max_core_heat_(3),
      level_(1),
      exp_(0),
      attack_range_(1),
      inventory_(12) {}

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
