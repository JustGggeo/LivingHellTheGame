#pragma once

#include <memory>

#include "Destructible.h"
#include "Item.h"

class DroppableChest : public Destructible {
 public:
  DroppableChest(int x, int y, std::unique_ptr<Item> item);

  std::unique_ptr<Item> TakeItem();
  bool HasItem() const;

 private:
  std::unique_ptr<Item> item_;
};
