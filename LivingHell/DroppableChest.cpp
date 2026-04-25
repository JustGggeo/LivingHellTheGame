#include "DroppableChest.h"

DroppableChest::DroppableChest(int x, int y, std::unique_ptr<Item> item)
    : Destructible(x, y, 3, 0, "Droppable Chest"), item_(std::move(item)) {}

std::unique_ptr<Item> DroppableChest::TakeItem() { return std::move(item_); }

bool DroppableChest::HasItem() const { return item_ != nullptr; }