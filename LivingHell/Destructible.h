#pragma once

#include <string>

#include "Entity.h"

class Destructible : public Entity {
 public:
  Destructible(int x, int y, int health, int exp_reward,
               const std::string& name);

  void TakeDamage(int dmg) override;

  int GetExpReward() const;
  const std::string& GetName() const;
  bool WasDestroyed() const;

 private:
  int exp_reward_;
  std::string name_;
  bool was_destroyed_;
};

class SoulAshurn : public Destructible {
 public:
  SoulAshurn(int x, int y);
};
