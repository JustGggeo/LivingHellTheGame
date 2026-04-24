#include "Chest.h"

#include "Player.h"

Chest::Chest(int x, int y, const std::string& name)
    : Destructible(x, y, 1, 0, name), is_opened_(false) {}

bool Chest::IsOpened() const { return is_opened_; }

WeaponChest::WeaponChest(int x, int y, std::unique_ptr<Weapon> weapon)
    : Chest(x, y, "Weapon Chest"), weapon_(std::move(weapon)) {}

void WeaponChest::Open(Player& player) {
  if (is_opened_ || !weapon_) return;
  is_opened_ = true;
  player.GetInventory().AddItem(std::move(weapon_));
}

ItemChest::ItemChest(int x, int y, std::unique_ptr<Consumable> item)
    : Chest(x, y, "Item Chest"), item_(std::move(item)) {}

void ItemChest::Open(Player& player) {
  if (is_opened_ || !item_) return;
  is_opened_ = true;
  player.GetInventory().AddItem(std::move(item_));
}
