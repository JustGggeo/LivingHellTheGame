#include "GameState.h"

GameState::GameState()
    : player_(1, 1),
      turn_system_(100),
      current_circle_(1),
      status_(GameStatus::kPlaying) {
  current_room_ = std::make_unique<Room>(30, 30, RoomType::kCombat);
}

void GameState::Init(const std::string& enemies_path,
                     const std::string& items_path) {
  database_.LoadEnemies(enemies_path);
  database_.LoadItems(items_path);
}

void GameState::ProcessAction(Action action) {
  if (status_ != GameStatus::kPlaying) return;
  turn_system_.ProcessTurn(action, player_, *current_room_);
  CheckStatus();
}

void GameState::CheckStatus() {
  if (turn_system_.IsGameOver()) {
    status_ = GameStatus::kDefeat;
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
