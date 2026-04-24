#include "Item.h"

Item::Item(const std::string& item_id, const std::string& name,
           int slots_required, ItemType type)
    : item_id_(item_id),
      name_(name),
      slots_required_(slots_required),
      type_(type) {}

const std::string& Item::GetItemId() const { return item_id_; }
const std::string& Item::GetName() const { return name_; }
int Item::GetSlotsRequired() const { return slots_required_; }
ItemType Item::GetType() const { return type_; }
