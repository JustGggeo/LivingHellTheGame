#include "Weapon.h"

#include "Player.h"

Weapon::Weapon(const std::string& item_id, const std::string& name,
               int min_range, int max_range, int damage, float syphon_percent)
    : Item(item_id, name, 1, ItemType::kWeapon),
      min_range_(min_range),
      max_range_(max_range),
      damage_(damage),
      syphon_percent_(syphon_percent) {}

void Weapon::Use(Player& player) {}

int Weapon::GetDamage() const { return damage_; }
int Weapon::GetMinRange() const { return min_range_; }
int Weapon::GetMaxRange() const { return max_range_; }
float Weapon::GetSyphonPercent() const { return syphon_percent_; }

bool Weapon::IsInRange(int dist) const {
  return dist >= min_range_ && dist <= max_range_;
}
