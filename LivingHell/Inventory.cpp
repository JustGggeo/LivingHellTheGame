#include "Inventory.h"

#include "Player.h"

Inventory::Inventory(int max_slots)
    : max_slots_(max_slots), used_slots_(0), active_index_(0) {}

bool Inventory::AddItem(std::unique_ptr<Item> item) {
  if (used_slots_ + item->GetSlotsRequired() > max_slots_) return false;
  used_slots_ += item->GetSlotsRequired();
  items_.push_back(std::move(item));
  return true;
}

void Inventory::RemoveItem(int index) {
  if (index < 0 || index >= static_cast<int>(items_.size())) return;
  used_slots_ -= items_[index]->GetSlotsRequired();
  items_.erase(items_.begin() + index);
  if (active_index_ >= static_cast<int>(items_.size())) active_index_ = 0;
}

bool Inventory::UseItem(int index, Player& player) {
  if (index < 0 || index >= static_cast<int>(items_.size())) return false;
  items_[index]->Use(player);
  if (items_[index]->GetType() == ItemType::kConsumable) RemoveItem(index);
  return true;
}

Item* Inventory::GetItem(int index) const {
  if (index < 0 || index >= static_cast<int>(items_.size())) return nullptr;
  return items_[index].get();
}

Item* Inventory::GetActiveItem() const { return GetItem(active_index_); }

void Inventory::SetActiveIndex(int index) {
  if (index >= 0 && index < static_cast<int>(items_.size()))
    active_index_ = index;
}

int Inventory::GetMaxSlots() const { return max_slots_; }
int Inventory::GetUsedSlots() const { return used_slots_; }
int Inventory::GetItemCount() const { return static_cast<int>(items_.size()); }
int Inventory::GetActiveIndex() const { return active_index_; }
bool Inventory::IsFull() const { return used_slots_ >= max_slots_; }
