#pragma once

#include "Item.h"

class Consumable : public Item {
 public:
  Consumable(const std::string& item_id, const std::string& name,
             EffectType effect_type, int effect_value);

  void Use(Player& player) override;

  EffectType GetEffectType() const;
  int GetEffectValue() const;

 private:
  EffectType effect_type_;
  int effect_value_;
};
