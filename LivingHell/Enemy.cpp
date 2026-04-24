#include "Enemy.h"

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
  UpdateState(player);
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

void Enemy::UpdateState(Player& player) {
  int dist = GetDistanceTo(player.GetX(), player.GetY());
  if (dist <= attack_range_) {
    state_ = EnemyState::kAttacking;
  } else if (dist <= 5) {
    state_ = EnemyState::kChasing;
  } else {
    state_ = EnemyState::kPatrolling;
  }
}

void Enemy::Patrol() {
  // Пока заглушка — позже добавим случайное перемещение
}

void Enemy::ChasePlayer(Player& player, Room& room) {
  int dx = 0, dy = 0;
  if (player.GetX() > x_)
    dx = 1;
  else if (player.GetX() < x_)
    dx = -1;
  if (player.GetY() > y_)
    dy = 1;
  else if (player.GetY() < y_)
    dy = -1;

  int new_x = x_ + dx;
  int new_y = y_ + dy;
  if (new_x >= 0 && new_x < room.GetWidth() && new_y >= 0 &&
      new_y < room.GetHeight() && room.GetTile(new_x, new_y).IsWalkable()) {
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

int Enemy::GetExpReward() const { return exp_reward_; }
int Enemy::GetHeatDamage() const { return heat_damage_; }
int Enemy::GetAttackRange() const { return attack_range_; }
EnemyState Enemy::GetState() const { return state_; }
const std::string& Enemy::GetEnemyId() const { return enemy_id_; }
