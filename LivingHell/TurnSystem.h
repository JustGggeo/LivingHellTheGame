#pragma once

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
  kWait
};

class TurnSystem {
 public:
  TurnSystem(int timer_limit);

  void ProcessTurn(Action action, Player& player, Room& room);
  bool IsTimeOut() const;
  bool IsGameOver() const;
  int GetTurnCount() const;
  int GetCurrentTimer() const;
  bool PlayerReachedExit() const;

 private:
  int turn_count_;
  int timer_limit_;
  int current_timer_;
  bool game_over_;
  bool reached_exit_;

  void ProcessPlayerAction(Action action, Player& player, Room& room);
  void ResolveEnemies(Player& player, Room& room);
  void CheckCoreOverheat(Player& player);
};
