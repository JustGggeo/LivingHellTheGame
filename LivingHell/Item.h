#pragma once

#include <string>

enum class ItemType { kWeapon, kConsumable, kSpecial };

enum class EffectType { kCool, kHeat, kHeal, kEmpower };

class Player;

class Item {
 public:
  Item(const std::string& item_id, const std::string& name, int slots_required,
       ItemType type);
  virtual ~Item() = default;

  virtual void Use(Player& player) = 0;

  const std::string& GetItemId() const;
  const std::string& GetName() const;
  int GetSlotsRequired() const;
  ItemType GetType() const;

 protected:
  std::string item_id_;
  std::string name_;
  int slots_required_;
  ItemType type_;
};
