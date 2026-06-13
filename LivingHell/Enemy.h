#pragma once

#include <string>

#include "Entity.h"

enum class EnemyState { kPatrolling, kChasing, kAttacking, kDead };

class Player;
class Room;

class Enemy : public Entity {
 public:
  Enemy(int x, int y, int health, int damage, int attack_range, int move_speed,
        int heat_damage, int exp_reward, const std::string& enemy_id);

  void TakeDamage(int dmg) override;
  void Act(Player& player, Room& room);

  int GetExpReward() const;
  int GetHeatDamage() const;
  int GetAttackRange() const;
  EnemyState GetState() const;
  const std::string& GetEnemyId() const;
  bool IsEnraged() const;

 private:
  std::string enemy_id_;
  int damage_;
  int attack_range_;
  int move_speed_;
  int heat_damage_;
  int exp_reward_;
  EnemyState state_;

  void UpdateState(Player& player, Room& room);

  void Patrol();
  void ChasePlayer(Player& player, Room& room);
  void AttackPlayer(Player& player);
  void BossAct(Player& player, Room& room);
  int GetDistanceTo(int target_x, int target_y) const;
  bool HasLineOfSight(int target_x, int target_y, Room& room) const;

  bool enraged_ = false;
};
