#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "GameDatabase.h"
#include "IAbilityObserver.h"
#include "LevelGenerator.h"
#include "Player.h"
#include "Room.h"
#include "TurnSystem.h"

enum class GameStatus { kPlaying, kVictory, kDefeat };

class GameState {
 public:
  GameState();

  void Init(const std::string& enemies_path, const std::string& items_path,
            const std::string& rooms_path, const std::string& playerstats_path);
  void ProcessAction(Action action);
  void Run(sf::RenderWindow& window);

  Player& GetPlayer();
  Room& GetCurrentRoom();
  GameStatus GetStatus() const;
  int GetCurrentCircle() const;
  int GetCurrentTimer() const;
  bool HandleInput(const sf::Event::KeyPressed& key);

 private:
  // database_ должна быть первой — player_ и turn_system_ получают на неё ссылку
  GameDatabase database_;
  Player player_;
  std::unique_ptr<Room> current_room_;
  TurnSystem turn_system_;
  int current_circle_;
  GameStatus status_;
  LevelGenerator level_generator_;

  void CheckStatus();
  void LoadNextRoom();
  void SpawnEnemies();
  void ApplyGeneratedRooms();

  // Observer для системы абилок
  AbilityLogger ability_logger_;
};
