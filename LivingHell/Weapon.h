#pragma once

#include "Item.h"

class Weapon : public Item {
 public:
  Weapon(const std::string& item_id, const std::string& name, int min_range,
         int max_range, int damage, float syphon_percent);

  void Use(Player& player) override;

  int GetDamage() const;
  int GetMinRange() const;
  int GetMaxRange() const;
  float GetSyphonPercent() const;
  bool IsInRange(int dist) const;

 private:
  int min_range_;
  int max_range_;
  int damage_;
  float syphon_percent_;
};
