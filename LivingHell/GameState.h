#pragma once

#include <memory>
#include <vector>

#include "GameDatabase.h"
#include "Player.h"
#include "Room.h"
#include "TurnSystem.h"

enum class GameStatus { kPlaying, kVictory, kDefeat };

class GameState {
 public:
  GameState();

  void Init(const std::string& enemies_path, const std::string& items_path);
  void ProcessAction(Action action);

  Player& GetPlayer();
  Room& GetCurrentRoom();
  GameStatus GetStatus() const;
  int GetCurrentCircle() const;
  int GetCurrentTimer() const;

 private:
  Player player_;
  std::unique_ptr<Room> current_room_;
  TurnSystem turn_system_;
  GameDatabase database_;
  int current_circle_;
  GameStatus status_;

  void CheckStatus();
  void LoadNextRoom();
};
