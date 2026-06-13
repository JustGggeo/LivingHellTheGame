#include "Enemy.h"

#include "Constants.h"
#include "Player.h"
#include "Room.h"

Enemy::Enemy(int x, int y, int health, int damage, int attack_range,
             int move_speed, int heat_damage, int exp_reward,
             const std::string& enemy_id)
    : Entity(x, y, health),
      enemy_id_(enemy_id),
      damage_(damage),
      attack_range_(attack_range),
      move_speed_(move_speed),
      heat_damage_(heat_damage),
      exp_reward_(exp_reward),
      state_(EnemyState::kPatrolling) {}

void Enemy::TakeDamage(int dmg) {
  Entity::TakeDamage(dmg);
  if (!IsAlive()) state_ = EnemyState::kDead;
}

void Enemy::Act(Player& player, Room& room) {
  if (state_ == EnemyState::kDead) return;
  if (enemy_id_ == "devil") {
    BossAct(player, room);
    return;
  }
  UpdateState(player, room);
  switch (state_) {
    case EnemyState::kPatrolling:
      Patrol();
      break;
    case EnemyState::kChasing:
      ChasePlayer(player, room);
      break;
    case EnemyState::kAttacking:
      AttackPlayer(player);
      break;
    default:
      break;
  }
}

void Enemy::BossAct(Player& player, Room& room) {
  if (!enraged_ && health_ <= max_health_ / 2) {
    enraged_ = true;
  }

  int dist = GetDistanceTo(player.GetX(), player.GetY());
  if (dist <= attack_range_) {
    AttackPlayer(player);
    if (enraged_) {
      AttackPlayer(player);
      int px = player.GetX(), py = player.GetY();
      for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
          if (dx == 0 && dy == 0) continue;
          if (px + dx == player.GetX() && py + dy == player.GetY())
            player.AddCoreHeat(heat_damage_);
        }
      }
    }
  } else {
    ChasePlayer(player, room);
  }
}

void Enemy::UpdateState(Player& player, Room& room) {
  int dist = GetDistanceTo(player.GetX(), player.GetY());
  if (dist <= attack_range_ &&
      HasLineOfSight(player.GetX(), player.GetY(), room)) {
    state_ = EnemyState::kAttacking;
  } else if (dist <= Constants::kEnemyVisionRange) {
    state_ = EnemyState::kChasing;
  } else {
    state_ = EnemyState::kPatrolling;
  }
}

void Enemy::Patrol() {}

void Enemy::ChasePlayer(Player& player, Room& room) {
  int dist = GetDistanceTo(player.GetX(), player.GetY());
  int dx = 0, dy = 0;
  if (player.GetX() > x_)
    dx = 1;
  else if (player.GetX() < x_)
    dx = -1;
  if (player.GetY() > y_)
    dy = 1;
  else if (player.GetY() < y_)
    dy = -1;

  // Если attack_range > 1 — держим дистанцию, отступаем если слишком близко
  if (attack_range_ > 1) {
    if (dist <= 1) {
      dx = -dx;
      dy = -dy;
    } else if (dist <= attack_range_) {
      return;
    }
  }

  int new_x = x_ + dx;
  int new_y = y_ + dy;

  bool tile_ok = new_x >= 0 && new_x < room.GetWidth() && new_y >= 0 &&
                 new_y < room.GetHeight() &&
                 room.GetTile(new_x, new_y).IsWalkable();

  bool blocked_by_destructible = false;
  for (const auto& d : room.GetDestructibles()) {
    if (d->IsAlive() && d->GetX() == new_x && d->GetY() == new_y) {
      blocked_by_destructible = true;
      break;
    }
  }

  if (tile_ok && !blocked_by_destructible) {
    x_ = new_x;
    y_ = new_y;
  }
}

void Enemy::AttackPlayer(Player& player) {
  player.TakeDamage(damage_);
  player.AddCoreHeat(heat_damage_);
}

int Enemy::GetDistanceTo(int target_x, int target_y) const {
  int dx = target_x - x_;
  int dy = target_y - y_;
  if (dx < 0) dx = -dx;
  if (dy < 0) dy = -dy;
  return dx > dy ? dx : dy;
}

bool Enemy::HasLineOfSight(int target_x, int target_y, Room& room) const {
  int x = x_, y = y_;
  int dx = target_x - x, dy = target_y - y;
  int steps = std::max(std::abs(dx), std::abs(dy));
  if (steps == 0) return true;
  for (int i = 1; i < steps; i++) {
    int cx = x + dx * i / steps;
    int cy = y + dy * i / steps;
    if (!room.GetTile(cx, cy).IsWalkable()) return false;
  }
  return true;
}

int Enemy::GetExpReward() const { return exp_reward_; }
int Enemy::GetHeatDamage() const { return heat_damage_; }
int Enemy::GetAttackRange() const { return attack_range_; }
EnemyState Enemy::GetState() const { return state_; }
const std::string& Enemy::GetEnemyId() const { return enemy_id_; }
bool Enemy::IsEnraged() const { return enraged_; }
