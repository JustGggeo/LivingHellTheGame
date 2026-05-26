#include "GameState.h"

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

#include "Constants.h"
#include "Consumable.h"
#include "DroppableChest.h"
#include "Renderer.h"

GameState::GameState()
    : player_(1, 1, database_),
      turn_system_(Constants::kTimerLimit, database_),
      level_generator_(Constants::kFieldSize, Constants::kFieldSize),
      current_circle_(1),
      status_(GameStatus::kPlaying) {
  srand(static_cast<unsigned int>(time(nullptr)));
  Init("enemies.csv", "items.csv", "rooms.csv", "playerstats.csv");
  player_.AddAbilityObserver(&ability_logger_);
  level_generator_.Generate(current_circle_, database_);
  current_room_ = std::make_unique<Room>(
      Constants::kFieldSize, Constants::kFieldSize, RoomType::kCombat);
  ApplyGeneratedRooms();
  SpawnEnemies();
}

void GameState::Init(const std::string& enemies_path,
                     const std::string& items_path,
                     const std::string& rooms_path,
                     const std::string& playerstats_path) {
  database_.LoadEnemies(enemies_path);
  database_.LoadItems(items_path);
  database_.LoadRooms(rooms_path);
  database_.LoadPlayerStats(playerstats_path);
}

void GameState::ProcessAction(Action action) {
  if (status_ != GameStatus::kPlaying) return;
  turn_system_.ProcessTurn(action, player_, *current_room_);
  CheckStatus();
}

void GameState::CheckStatus() {
  if (status_ != GameStatus::kPlaying) return;

  if (turn_system_.IsGameOver()) {
    status_ = GameStatus::kDefeat;
    return;
  }
  if (turn_system_.PlayerReachedExit()) {
    status_ = GameStatus::kVictory;
    return;
  }
  if (current_room_->IsCleared() && current_circle_ >= 3) {
    status_ = GameStatus::kVictory;
  }
}

void GameState::LoadNextRoom() {
  current_circle_++;
  current_room_ = std::make_unique<Room>(
      Constants::kFieldSize, Constants::kFieldSize, RoomType::kCombat);
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
    if (data.circle != current_circle_) continue;

    int x = 0, y = 0;
    bool found = false;
    for (int attempt = 0; attempt < Constants::kMaxSpawnAttempts; attempt++) {
      x = Constants::kSpawnMargin +
          rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
      y = Constants::kSpawnMargin +
          rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
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

    int soul_count = Constants::kSoulAshurnCount +
                     rand() % Constants::kSoulSpawnVariation;
    for (int i = 0; i < soul_count; i++) {
      bool found = false;
      for (int attempt = 0; attempt < Constants::kMaxSpawnAttempts; attempt++) {
        int x = Constants::kSpawnMargin +
                rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
        int y = Constants::kSpawnMargin +
                rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
        if (current_room_->GetTile(x, y).IsWalkable() &&
            !(x == player_.GetX() && y == player_.GetY())) {
          current_room_->AddDestructible(std::make_unique<SoulAshurn>(x, y));
          found = true;
          break;
        }
      }
    }
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
    player_ = Player(start->x + 1, start->y + 1, database_);
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
  // Спавним сундук в случайной проходимой клетке
  bool chest_placed = false;
  for (int attempt = 0; attempt < Constants::kMaxSpawnAttempts && !chest_placed;
       attempt++) {
    int x = Constants::kSpawnMargin +
            rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
    int y = Constants::kSpawnMargin +
            rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
    if (current_room_->GetTile(x, y).IsWalkable()) {
      auto chest = std::make_unique<DroppableChest>(
          x, y,
          std::make_unique<Consumable>("khladagent", "Хладагент",
                                       EffectType::kCool, 4));
      current_room_->AddDestructible(std::move(chest));
      chest_placed = true;
    }
  }
}

bool GameState::HandleInput(const sf::Event::KeyPressed& key) {
  if (key.code == sf::Keyboard::Key::Escape) return true;
  if (status_ != GameStatus::kPlaying) return false;

  Action action = Action::kWait;
  switch (key.code) {
    case sf::Keyboard::Key::W:
      action = Action::kMoveUp;
      break;
    case sf::Keyboard::Key::S:
      action = Action::kMoveDown;
      break;
    case sf::Keyboard::Key::A:
      action = Action::kMoveLeft;
      break;
    case sf::Keyboard::Key::D:
      action = Action::kMoveRight;
      break;
    case sf::Keyboard::Key::Q:
      action = Action::kMoveUpLeft;
      break;
    case sf::Keyboard::Key::E:
      action = Action::kMoveUpRight;
      break;
    case sf::Keyboard::Key::Z:
      action = Action::kMoveDownLeft;
      break;
    case sf::Keyboard::Key::C:
      action = Action::kMoveDownRight;
      break;
    case sf::Keyboard::Key::X:
      action = Action::kUseItem;
      break;
    case sf::Keyboard::Key::Num1:
      action = Action::kUseAbility1;
      break;
    case sf::Keyboard::Key::Num2:
      action = Action::kUseAbility2;
      break;
    case sf::Keyboard::Key::Num3:
      action = Action::kUseAbility3;
      break;
    case sf::Keyboard::Key::Num4:
      action = Action::kUseAbility4;
      break;
    case sf::Keyboard::Key::Num5:
      action = Action::kUseAbility5;
      break;
    default:
      break;
  }
  ProcessAction(action);
  return false;
}

void GameState::Run(sf::RenderWindow& window) {
  Renderer renderer(window, "cour.ttf");
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) window.close();
      if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        if (HandleInput(*key)) window.close();
    }
    window.clear(sf::Color(20, 20, 20));
    renderer.Draw(*this);
    window.display();
  }
}
