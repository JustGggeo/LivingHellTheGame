#include "Consumable.h"

#include "Player.h"

Consumable::Consumable(const std::string& item_id, const std::string& name,
                       EffectType effect_type, int effect_value)
    : Item(item_id, name, 1, ItemType::kConsumable),
      effect_type_(effect_type),
      effect_value_(effect_value) {}

void Consumable::Use(Player& player) {
  switch (effect_type_) {
    case EffectType::kHeal:
      player.TakeDamage(-effect_value_);
      break;
    case EffectType::kCool:
      player.AddCoreHeat(-effect_value_);
      break;
    case EffectType::kHeat:
      player.AddCoreHeat(effect_value_);
      break;
    case EffectType::kEmpower:
      // реализуем позже через активный слот
      break;
    default:
      break;
  }
}

EffectType Consumable::GetEffectType() const { return effect_type_; }
int Consumable::GetEffectValue() const { return effect_value_; }
