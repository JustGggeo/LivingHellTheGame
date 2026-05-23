#include "Abilities.h"

#include "Enemy.h"
#include "Player.h"
#include "Room.h"

// Враги на соседних клетках
std::vector<Enemy*> GetAdjacentEnemies(Player& player, Room& room) {
  std::vector<Enemy*> result;
  int px = player.GetX(), py = player.GetY();
  for (auto& e : room.GetEnemies()) {
    int dx = std::abs(e->GetX() - px);
    int dy = std::abs(e->GetY() - py);
    if (dx <= 1 && dy <= 1 && (dx + dy > 0) && e->IsAlive())
      result.push_back(e.get());
  }
  return result;
}

// 1. Обычный удар
int BasicAttack::GetDamage(const Player& player) const {
  int base = 1 + (player.GetLevel() - 1);  // растёт с уровнем
  return base + player.GetAttackDamageBonus();
}

int BasicAttack::GetRange(const Player& player) const {
  return 1 + player.GetAttackRangeBonus();
}

void BasicAttack::Activate(Player& player, Room& room) {
  if (!IsReady()) return;
  int range = GetRange(player);
  int dmg = GetDamage(player);

  if (player.HasEmissionBuff()) {
    dmg += player.GetCoreHeat();
    player.ClearEmissionBuff();
  }

  int px = player.GetX(), py = player.GetY();
  for (auto& e : room.GetEnemies()) {
    int dx = std::abs(e->GetX() - px);
    int dy = std::abs(e->GetY() - py);
    if (dx <= range && dy <= range && e->IsAlive()) e->TakeDamage(dmg);
  }

  for (auto& obj : room.GetDestructibles()) {
    int dx = std::abs(obj->GetX() - px);
    int dy = std::abs(obj->GetY() - py);
    if (dx <= range && dy <= range && obj->IsAlive()) {
      obj->TakeDamage(dmg);
      if (!obj->IsAlive()) player.GainExp(obj->GetExpReward());
    }
  }

  if (current_cooldown_ == 0) current_cooldown_ = cooldown_;
}

// 2. Эмиссия
void Emission::Activate(Player& player, Room& room) {
  if (!IsReady()) return;
  player.SetEmissionBuff(true);
  current_cooldown_ = cooldown_;
}

// 3. Фазировка
void Phasing::Activate(Player& player, Room& room) {
  if (!IsReady()) return;
  player.AddCoreHeat(2);
  player.TakeDamage(1);

  for (Enemy* e : GetAdjacentEnemies(player, room)) e->TakeDamage(5);

  current_cooldown_ = cooldown_;
}

// 4. Деформация
void Deformation::Activate(Player& player, Room& room) {
  if (!IsReady()) return;
  int heat = player.GetCoreHeat();
  int heal = heat / 3;  // за каждые 3 единицы — 1 HP
  if (heal > 0) {
    player.TakeDamage(-heal);   // отрицательный урон = лечение
    player.AddCoreHeat(-heat);  // снимаем всё тепло
  }
  current_cooldown_ = cooldown_;
}

// 5. Дезинтеграция
void Disintegration::Activate(Player& player, Room& room) {
  if (!IsReady()) return;
  int heat = player.GetCoreHeat();
  int dmg = (heat / 2) * 4;  // за каждые 2 единицы — 4 урона

  int px = player.GetX(), py = player.GetY();
  // 4 направления: вверх, вниз, влево, вправо
  const int dirs[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
  for (auto& dir : dirs) {
    for (int dist = 1; dist <= 5; dist++) {
      int tx = px + dir[0] * dist;
      int ty = py + dir[1] * dist;
      for (auto& e : room.GetEnemies()) {
        if (e->GetX() == tx && e->GetY() == ty && e->IsAlive())
          e->TakeDamage(dmg);
      }
    }
  }

  player.AddCoreHeat(5);
  current_cooldown_ = cooldown_;
}
