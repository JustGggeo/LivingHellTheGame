#pragma once

class Entity {
 public:
  Entity(int x, int y, int max_health);
  virtual ~Entity() = default;

  virtual void TakeDamage(int dmg);
  virtual bool Move(int dx, int dy);
  void Teleport(int x, int y);

  int GetHealth() const;
  int GetMaxHealth() const;
  int GetX() const;
  int GetY() const;
  bool IsAlive() const;

 protected:
  int x_;
  int y_;
  int health_;
  int max_health_;
};
