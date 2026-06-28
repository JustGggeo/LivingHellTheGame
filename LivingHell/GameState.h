#pragma once

#include <SDL.h>
#include <memory>
#include <vector>

#include "Consumable.h"
#include "GameDatabase.h"
#include "Item.h"
#include "Weapon.h"
#include "IAbilityObserver.h"
#include "LevelGenerator.h"
#include "Player.h"
#include "Room.h"
#include "TurnSystem.h"

enum class GameStatus { kPlaying, kVictory, kDefeat };

enum class TransitionPhase { kFadeOut, kHold, kFadeIn };

struct LevelTransition {
  bool active = false;
  TransitionPhase phase = TransitionPhase::kFadeOut;
  float alpha = 0.f;
  int target_circle = 1;
};

class GameState {
 public:
  GameState();

  void Init(const std::string& enemies_path, const std::string& items_path,
            const std::string& rooms_path, const std::string& playerstats_path);
  void ProcessAction(Action action);
  bool Run(SDL_Renderer* renderer);
  void NextLevel();

  Player& GetPlayer();
  Room& GetCurrentRoom();
  GameStatus GetStatus() const;
  int GetCurrentCircle() const;
  int GetCurrentTimer() const;
  bool HandleInput(const SDL_KeyboardEvent& key);
  const LevelTransition& GetTransition() const;

 private:
  // database_ должна быть первой — player_ и turn_system_ получают на неё
  // ссылку
  GameDatabase database_;
  Player player_;
  std::unique_ptr<Room> current_room_;
  TurnSystem turn_system_;
  int current_circle_;
  GameStatus status_;
  LevelGenerator level_generator_;

  void CheckStatus();
  void LoadNextRoom();
  void RestartLevel();
  void SpawnEnemies();
  void ApplyGeneratedRooms();
  std::unique_ptr<Item> CreateRandomItem();
  std::unique_ptr<Item> CreateItemFromData(const ItemData& data);
  void SnapshotInventory();
  void RestoreInventory();
  bool IsTileFree(int x, int y) const;

  LevelTransition transition_;
  bool pending_next_level_ = false;
  bool pending_restart_level_ = false;
  bool restart_requested_ = false;
  std::vector<std::string> used_item_ids_;
  std::vector<std::string> inventory_snapshot_ids_;

  // Observer для системы абилок
  AbilityLogger ability_logger_;
};
