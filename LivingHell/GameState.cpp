#include "GameState.h"

#include <cstdlib>
#include <ctime>

GameState::GameState()
    : player_(1, 1),
      turn_system_(100),
      level_generator_(30, 30),
      current_circle_(1),
      status_(GameStatus::kPlaying) {
  srand(static_cast<unsigned int>(time(nullptr)));
  Init("enemies.csv", "items.csv", "rooms.csv");
  level_generator_.Generate(current_circle_, database_);
  current_room_ = std::make_unique<Room>(30, 30, RoomType::kCombat);
  ApplyGeneratedRooms();
  SpawnEnemies();
}

void GameState::Init(const std::string& enemies_path,
                     const std::string& items_path,
                     const std::string& rooms_path) {
  database_.LoadEnemies(enemies_path);
  database_.LoadItems(items_path);
  database_.LoadRooms(rooms_path);
}

void GameState::ProcessAction(Action action) {
  if (status_ != GameStatus::kPlaying) return;
  turn_system_.ProcessTurn(action, player_, *current_room_);
  CheckStatus();
}

void GameState::CheckStatus() {
  if (status_ != GameStatus::kPlaying)
    return;  // <-- добавить, чтоб не перезаписывать статус

  if (turn_system_.IsGameOver()) {
    status_ = GameStatus::kDefeat;
    return;
  }
  if (turn_system_.PlayerReachedExit()) {  // <-- добавить
    status_ = GameStatus::kVictory;
    return;
  }
  if (current_room_->IsCleared() && current_circle_ >= 3) {
    status_ = GameStatus::kVictory;
  }
}

void GameState::LoadNextRoom() {
  current_circle_++;
  current_room_ = std::make_unique<Room>(30, 30, RoomType::kCombat);
}

Player& GameState::GetPlayer() { return player_; }
Room& GameState::GetCurrentRoom() { return *current_room_; }
GameStatus GameState::GetStatus() const { return status_; }
int GameState::GetCurrentCircle() const { return current_circle_; }
int GameState::GetCurrentTimer() const {
  return turn_system_.GetCurrentTimer();
}

void GameState::SpawnEnemies() {
  for (const auto& data : database_.GetAllEnemies()) {
    if (std::stoi(data.circle) != current_circle_) continue;

    int x = 0, y = 0;
    bool found = false;
    for (int attempt = 0; attempt < 100; attempt++) {
      x = 2 + rand() % 26;
      y = 2 + rand() % 26;
      if (current_room_->GetTile(x, y).IsWalkable() &&
          !(x == player_.GetX() && y == player_.GetY())) {
        found = true;
        break;
      }
    }
    if (!found) continue;

    auto enemy = std::make_unique<Enemy>(
        x, y, data.health, data.damage, data.attack_range, data.move_speed,
        data.heat_damage, data.exp_reward, data.id);
    current_room_->AddEnemy(std::move(enemy));
  }
}

void GameState::ApplyGeneratedRooms() {
  const auto& placements = level_generator_.GetPlacements();
  for (const auto& p : placements) {
    for (int y = p.y; y < p.y + p.height; y++) {
      for (int x = p.x; x < p.x + p.width; x++) {
        bool is_wall = (x == p.x || x == p.x + p.width - 1 || y == p.y ||
                        y == p.y + p.height - 1);
        current_room_->GetTile(x, y).SetType(is_wall ? TileType::kWall
                                                     : TileType::kFloor);
      }
    }
  }

  const RoomPlacement* start = level_generator_.GetStartRoom();
  if (start) {
    player_ = Player(start->x + 1, start->y + 1);
  }

  for (int i = 1; i < static_cast<int>(placements.size()); i++) {
    int x1 = placements[i - 1].x + placements[i - 1].width / 2;
    int y1 = placements[i - 1].y + placements[i - 1].height / 2;
    int x2 = placements[i].x + placements[i].width / 2;
    int y2 = placements[i].y + placements[i].height / 2;

    int step_x = (x2 > x1) ? 1 : -1;
    for (int x = x1; x != x2; x += step_x)
      current_room_->GetTile(x, y1).SetType(TileType::kFloor);

    int step_y = (y2 > y1) ? 1 : -1;
    for (int y = y1; y != y2 + step_y; y += step_y)
      current_room_->GetTile(x2, y).SetType(TileType::kFloor);
  }

  const RoomPlacement* exit = level_generator_.GetExitRoom();
  if (exit) {
    current_room_
        ->GetTile(exit->x + exit->width / 2, exit->y + exit->height / 2)
        .SetType(TileType::kExit);
  } else {
    bool placed = false;
    for (int y = 27; y >= 2 && !placed; y--)
      for (int x = 27; x >= 2 && !placed; x--)
        if (current_room_->GetTile(x, y).GetType() == TileType::kFloor) {
          current_room_->GetTile(x, y).SetType(TileType::kExit);
          placed = true;
        }
  }
}
