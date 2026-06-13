#pragma once

#include "GameDatabase.h"
#include "Player.h"
#include "Room.h"

enum class Action {
  kMoveUp,
  kMoveDown,
  kMoveLeft,
  kMoveRight,
  kMoveUpLeft,
  kMoveUpRight,
  kMoveDownLeft,
  kMoveDownRight,
  kAttack,
  kUseAbility,
  kUseItem,
  kWait,
  kUseAbility1,
  kUseAbility2,
  kUseAbility3,
  kUseAbility4,
  kUseAbility5
};

class TurnSystem {
 public:
  TurnSystem(int timer_limit, const GameDatabase& db);

  void ProcessTurn(Action action, Player& player, Room& room);
  bool IsTimeOut() const;
  bool IsGameOver() const;
  int GetTurnCount() const;
  int GetCurrentTimer() const;
  bool PlayerReachedExit() const;
  void Reset();

 private:
  int turn_count_;
  int timer_limit_;
  int current_timer_;
  bool game_over_;
  bool reached_exit_;
  const GameDatabase& db_;

  void ProcessPlayerAction(Action action, Player& player, Room& room);
  void ResolveEnemies(Player& player, Room& room);
  void CheckCoreOverheat(Player& player);
};
