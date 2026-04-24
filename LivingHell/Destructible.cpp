#include "Destructible.h"

Destructible::Destructible(int x, int y, int health, int exp_reward,
                           const std::string& name)
    : Entity(x, y, health),
      exp_reward_(exp_reward),
      name_(name),
      was_destroyed_(false) {}

void Destructible::TakeDamage(int dmg) {
  Entity::TakeDamage(dmg);
  if (!IsAlive() && !was_destroyed_) {
    was_destroyed_ = true;
  }
}

int Destructible::GetExpReward() const { return exp_reward_; }
const std::string& Destructible::GetName() const { return name_; }
bool Destructible::WasDestroyed() const { return was_destroyed_; }

SoulAshurn::SoulAshurn(int x, int y)
    : Destructible(x, y, 1, 1, "Soul Ashurn") {}
