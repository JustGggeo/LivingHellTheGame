#include "Entity.h"

Entity::Entity(int x, int y, int max_health)
    : x_(x), y_(y), health_(max_health), max_health_(max_health) {}

void Entity::TakeDamage(int dmg) {
  health_ -= dmg;
  if (health_ < 0) health_ = 0;
  if (health_ > max_health_) health_ = max_health_;
}


bool Entity::Move(int dx, int dy) {
  x_ += dx;
  y_ += dy;
  return true;
}

int Entity::GetHealth() const { return health_; }
int Entity::GetMaxHealth() const { return max_health_; }
int Entity::GetX() const { return x_; }
int Entity::GetY() const { return y_; }
bool Entity::IsAlive() const { return health_ > 0; }
void Entity::Teleport(int x, int y) { x_ = x; y_ = y; }
