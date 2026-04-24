#include "TurnSystem.h"

TurnSystem::TurnSystem(int timer_limit)
    : turn_count_(0),
      timer_limit_(timer_limit),
      current_timer_(timer_limit),
      game_over_(false) {}

void TurnSystem::ProcessTurn(Action action, Player& player, Room& room) {
  if (game_over_) return;

  ProcessPlayerAction(action, player, room);
  CheckCoreOverheat(player);
  if (game_over_) return;

  ResolveEnemies(player, room);
  room.RemoveDeadEnemies(player);

  current_timer_--;
  turn_count_++;

  if (!player.IsAlive() || current_timer_ <= 0) game_over_ = true;
}

void TurnSystem::ProcessPlayerAction(Action action, Player& player,
                                     Room& room) {
  int dx = 0;
  int dy = 0;
  switch (action) {
    case Action::kMoveUp:
      dy = -1;
      break;
    case Action::kMoveDown:
      dy = 1;
      break;
    case Action::kMoveLeft:
      dx = -1;
      break;
    case Action::kMoveRight:
      dx = 1;
      break;
    case Action::kMoveUpLeft:
      dx = -1;
      dy = -1;
      break;
    case Action::kMoveUpRight:
      dx = 1;
      dy = -1;
      break;
    case Action::kMoveDownLeft:
      dx = -1;
      dy = 1;
      break;
    case Action::kMoveDownRight:
      dx = 1;
      dy = 1;
      break;
    case Action::kWait:
      player.AddCoreHeat(1);
      return;
    case Action::kUseItem:
      player.UseItem(player.GetInventory().GetActiveIndex());
      return;
    default:
      return;
  }

  int new_x = player.GetX() + dx;
  int new_y = player.GetY() + dy;
  if (new_x >= 0 && new_x < room.GetWidth() && new_y >= 0 &&
      new_y < room.GetHeight() && room.GetTile(new_x, new_y).IsWalkable()) {
    player.Move(dx, dy);
  } else {
    player.AddCoreHeat(1);  // штраф за неудачный ход
  }
}

void TurnSystem::ResolveEnemies(Player& player, Room& room) {
  for (auto& enemy : room.GetEnemies()) {
    if (enemy->IsAlive()) enemy->Act(player);
  }
}

void TurnSystem::CheckCoreOverheat(Player& player) {
  if (player.GetCoreHeat() >= player.GetMaxCoreHeat()) {
    player.TakeDamage(player.GetMaxHealth());
    game_over_ = true;
  }
}

bool TurnSystem::IsTimeOut() const { return current_timer_ <= 0; }
bool TurnSystem::IsGameOver() const { return game_over_; }
int TurnSystem::GetTurnCount() const { return turn_count_; }
int TurnSystem::GetCurrentTimer() const { return current_timer_; }
