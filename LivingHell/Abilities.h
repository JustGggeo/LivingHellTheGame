#pragma once
#include "Ability.h"

// 1. Обычный удар (уровень 1)
class BasicAttack : public Ability {
 public:
  BasicAttack() : Ability("Basic Attack", 0) {}
  void Activate(Player& player, Room& room) override;

 private:
  int GetDamage(const Player& player) const;
  int GetRange(const Player& player) const;
};

// 2. Эмиссия (уровень 2)
class Emission : public Ability {
 public:
  Emission() : Ability("Emission", 3) {}
  void Activate(Player& player, Room& room) override;
};

// 3. Фазировка (уровень 3)
class Phasing : public Ability {
 public:
  Phasing() : Ability("Phasing", 4) {}
  void Activate(Player& player, Room& room) override;
};

// 4. Деформация (уровень 3)
class Deformation : public Ability {
 public:
  Deformation() : Ability("Deformation", 4) {}
  void Activate(Player& player, Room& room) override;
};

// 5. Дезинтеграция (уровень 4)
class Disintegration : public Ability {
 public:
  Disintegration() : Ability("Disintegration", 5) {}
  void Activate(Player& player, Room& room) override;
};
