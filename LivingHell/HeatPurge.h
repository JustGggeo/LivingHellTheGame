#pragma once
#include "Ability.h"

class HeatPurge : public Ability {
 public:
  HeatPurge() : Ability("Heat Purge", 5) {}
  void Activate(Player& player, Room& room) override;
};