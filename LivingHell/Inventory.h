#pragma once

#include <memory>
#include <vector>

#include "Item.h"

class Player;

class Inventory {
 public:
  explicit Inventory(int max_slots);

  bool AddItem(std::unique_ptr<Item> item);
  void RemoveItem(int index);
  bool UseItem(int index, Player& player);

  Item* GetItem(int index) const;
  Item* GetActiveItem() const;
  void SetActiveIndex(int index);

  int GetMaxSlots() const;
  int GetUsedSlots() const;
  int GetItemCount() const;
  int GetActiveIndex() const;
  bool IsFull() const;

 private:
  int max_slots_;
  int used_slots_;
  int active_index_;
  std::vector<std::unique_ptr<Item>> items_;
};
