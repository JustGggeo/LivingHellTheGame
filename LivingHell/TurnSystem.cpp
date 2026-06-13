#include "TurnSystem.h"

#include "Constants.h"
#include "DroppableChest.h"
#include "Player.h"

TurnSystem::TurnSystem(int timer_limit, const GameDatabase& db)
    : turn_count_(0),
      timer_limit_(timer_limit),
      current_timer_(timer_limit),
      game_over_(false),
      reached_exit_(false),
      db_(db) {}

void TurnSystem::ProcessTurn(Action action, Player& player, Room& room) {
  if (game_over_) return;

  ProcessPlayerAction(action, player, room);

  // Эффект тайла под игроком
  switch (room.GetTile(player.GetX(), player.GetY()).GetType()) {
    case TileType::kMagma: player.AddCoreHeat(Constants::kMagmaHeat); break;
    case TileType::kLava:  player.AddCoreHeat(Constants::kLavaHeat);  break;
    case TileType::kAsh:   player.AddCoreHeat(-Constants::kAshCool);  break;
    case TileType::kIce:   player.AddCoreHeat(-Constants::kIceCool);  break;
    default: break;
  }

  CheckCoreOverheat(player);
  if (game_over_) return;

  ResolveEnemies(player, room);
  room.RemoveDeadEnemies(player);

  // Дроп предметов из уничтоженных сундуков
  for (auto& obj : room.GetDestructibles()) {
    auto* chest = dynamic_cast<DroppableChest*>(obj.get());
    if (chest && !chest->IsAlive() && chest->HasItem()) {
      room.AddDroppedItem(chest->GetX(), chest->GetY(), chest->TakeItem());
    }
  }

  current_timer_--;
  turn_count_++;
  player.TickAbilities();

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
    case Action::kUseAbility1:
      player.UseAbility(0, room);
      return;
    case Action::kUseAbility2:
      player.UseAbility(1, room);
      return;
    case Action::kUseAbility3:
      player.UseAbility(2, room);
      return;
    case Action::kUseAbility4:
      player.UseAbility(3, room);
      return;
    case Action::kUseAbility5:
      player.UseAbility(4, room);
      return;

    default:
      return;
  }

  int new_x = player.GetX() + dx;
  int new_y = player.GetY() + dy;
  bool tile_ok = new_x >= 0 && new_x < room.GetWidth() && new_y >= 0 &&
                 new_y < room.GetHeight() &&
                 room.GetTile(new_x, new_y).IsWalkable();

  bool enemy_on_tile = false;
  for (const auto& enemy : room.GetEnemies()) {
    if (enemy->IsAlive() && enemy->GetX() == new_x && enemy->GetY() == new_y) {
      enemy_on_tile = true;
      break;
    }
  }

  if (tile_ok && !enemy_on_tile) {
    player.Move(dx, dy);
    TileType stepped = room.GetTile(new_x, new_y).GetType();
    if (stepped == TileType::kKey) {
      player.PickUpKey();
      room.GetTile(new_x, new_y).SetType(TileType::kFloor);
    } else if (stepped == TileType::kExit && player.HasKey()) {
      player.ConsumeKey();
      reached_exit_ = true;
    }
    if (room.HasItemAt(new_x, new_y)) {
      auto item = room.PickUpItemAt(new_x, new_y);
      if (item) player.GetInventory().AddItem(std::move(item));
    }

  } else {
    player.AddCoreHeat(1);
  }
}

void TurnSystem::ResolveEnemies(Player& player, Room& room) {
  for (auto& enemy : room.GetEnemies()) {
    if (enemy->IsAlive()) enemy->Act(player, room);
  }
}

void TurnSystem::CheckCoreOverheat(Player& player) {
  if (player.GetCoreHeat() >= player.GetMaxCoreHeat()) {
    player.TakeDamage(player.GetMaxHealth());
    game_over_ = true;
  }
}

void TurnSystem::Reset() {
  turn_count_ = 0;
  current_timer_ = timer_limit_;
  game_over_ = false;
  reached_exit_ = false;
}

bool TurnSystem::IsTimeOut() const { return current_timer_ <= 0; }
bool TurnSystem::IsGameOver() const { return game_over_; }
int TurnSystem::GetTurnCount() const { return turn_count_; }
int TurnSystem::GetCurrentTimer() const { return current_timer_; }
bool TurnSystem::PlayerReachedExit() const { return reached_exit_; }
