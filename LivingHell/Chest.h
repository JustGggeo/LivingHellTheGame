#pragma once

#include <memory>

#include "Consumable.h"
#include "Destructible.h"
#include "Item.h"
#include "Weapon.h"

class Player;

class Chest : public Destructible {
 public:
  Chest(int x, int y, const std::string& name);

  virtual void Open(Player& player) = 0;
  bool IsOpened() const;

 protected:
  bool is_opened_;
};

class WeaponChest : public Chest {
 public:
  WeaponChest(int x, int y, std::unique_ptr<Weapon> weapon);
  void Open(Player& player) override;

 private:
  std::unique_ptr<Weapon> weapon_;
};

class ItemChest : public Chest {
 public:
  ItemChest(int x, int y, std::unique_ptr<Consumable> item);
  void Open(Player& player) override;

 private:
  std::unique_ptr<Consumable> item_;
};
