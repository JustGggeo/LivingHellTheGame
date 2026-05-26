#include "Abilities.h"

#include "Constants.h"
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
  int base = Constants::kPlayerInitialAttackRange + (player.GetLevel() - 1);
  return base + player.GetAttackDamageBonus();
}

int BasicAttack::GetRange(const Player& player) const {
  return Constants::kPlayerInitialAttackRange + player.GetAttackRangeBonus();
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
  player.AddCoreHeat(Constants::kPhasingHeatGain);
  player.TakeDamage(Constants::kPhasingSelfDamage);

  for (Enemy* e : GetAdjacentEnemies(player, room))
    e->TakeDamage(Constants::kPhasingAoeDamage);

  current_cooldown_ = cooldown_;
}

// 4. Деформация
void Deformation::Activate(Player& player, Room& room) {
  if (!IsReady()) return;
  int heat = player.GetCoreHeat();
  int heal = heat / Constants::kDeformationHeatPerHp;
  if (heal > 0) {
    player.TakeDamage(-heal);
    player.AddCoreHeat(-heat);
  }
  current_cooldown_ = cooldown_;
}

// 5. Дезинтеграция
void Disintegration::Activate(Player& player, Room& room) {
  if (!IsReady()) return;
  int heat = player.GetCoreHeat();
  int dmg = (heat / Constants::kDisintegrationHeatDivisor) *
            Constants::kDisintegrationDamageMultiplier;

  int px = player.GetX(), py = player.GetY();
  const int dirs[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
  for (auto& dir : dirs) {
    for (int dist = 1; dist <= Constants::kDisintegrationRayLength; dist++) {
      int tx = px + dir[0] * dist;
      int ty = py + dir[1] * dist;
      for (auto& e : room.GetEnemies()) {
        if (e->GetX() == tx && e->GetY() == ty && e->IsAlive())
          e->TakeDamage(dmg);
      }
    }
  }

  player.AddCoreHeat(Constants::kDisintegrationHeatGain);
  current_cooldown_ = cooldown_;
}
