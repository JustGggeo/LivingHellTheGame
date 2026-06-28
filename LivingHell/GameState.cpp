#include "GameState.h"

#include <cstdlib>
#include <ctime>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Constants.h"
#include "DroppableChest.h"
#include "Renderer.h"

GameState::GameState()
    : player_(1, 1, database_),
      turn_system_(Constants::kTimerLimit, database_),
      level_generator_(Constants::kFieldSize, Constants::kFieldSize),
      current_circle_(1),
      status_(GameStatus::kPlaying) {
  srand(static_cast<unsigned int>(time(nullptr)));
  Init("data/enemies.csv", "data/items.csv", "data/rooms.csv",
      "data/playerstats.csv");
  transition_ = {true, TransitionPhase::kHold, 255.f, 1};
  player_.AddAbilityObserver(&ability_logger_);
  level_generator_.Generate(current_circle_, database_);
  current_room_ = std::make_unique<Room>(
      Constants::kFieldSize, Constants::kFieldSize, RoomType::kCombat);
  ApplyGeneratedRooms();
  SpawnEnemies();
  player_.SnapshotProgress();
  SnapshotInventory();
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
  if (status_ != GameStatus::kPlaying || transition_.active) return;
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
    if (current_circle_ < 3) {
      transition_ = {true, TransitionPhase::kFadeOut, 0.f, current_circle_ + 1};
      pending_next_level_ = true;
    } else {
      status_ = GameStatus::kVictory;
    }
    return;
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
  if (current_circle_ == 3) {
    const EnemyData* boss = database_.GetEnemyData("devil");
    if (boss) {
      for (int attempt = 0; attempt < Constants::kMaxSpawnAttempts; attempt++) {
        int x = Constants::kSpawnMargin +
                rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
        int y = Constants::kSpawnMargin +
                rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
        if (IsTileFree(x, y)) {
          current_room_->AddEnemy(std::make_unique<Enemy>(
              x, y, boss->health, boss->damage, boss->attack_range,
              boss->move_speed, boss->heat_damage, boss->exp_reward, boss->id));
          break;
        }
      }
    }
  }

  std::vector<const EnemyData*> pool;
  for (const auto& data : database_.GetAllEnemies()) {
    if (data.circle <= current_circle_ && data.id != "devil")
      pool.push_back(&data);
  }
  if (pool.empty()) return;

  const RoomData* room_data = database_.GetRoomData("combat");
  int min_enemies = room_data ? room_data->min_enemies : 2;
  int count = min_enemies + rand() % current_circle_;

  for (int i = 0; i < count; i++) {
    const EnemyData& data = *pool[rand() % pool.size()];
    for (int attempt = 0; attempt < Constants::kMaxSpawnAttempts; attempt++) {
      int x = Constants::kSpawnMargin +
               rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
      int y = Constants::kSpawnMargin +
               rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
      if (IsTileFree(x, y)) {
        current_room_->AddEnemy(std::make_unique<Enemy>(
            x, y, data.health, data.damage, data.attack_range, data.move_speed,
            data.heat_damage, data.exp_reward, data.id));
        break;
      }
    }
  }

  int soul_count =
      Constants::kSoulAshurnCount + rand() % Constants::kSoulSpawnVariation;
  for (int i = 0; i < soul_count; i++) {
    for (int attempt = 0; attempt < Constants::kMaxSpawnAttempts; attempt++) {
      int x = Constants::kSpawnMargin +
               rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
      int y = Constants::kSpawnMargin +
               rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
      if (IsTileFree(x, y)) {
        current_room_->AddDestructible(std::make_unique<SoulAshurn>(x, y));
        break;
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
    player_.Teleport(start->x + 1, start->y + 1);
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
  std::vector<TileType> special_pool = {TileType::kAsh};
  if (current_circle_ >= 2) special_pool.push_back(TileType::kMagma);
  if (current_circle_ >= 2) special_pool.push_back(TileType::kIce);
  if (current_circle_ >= 3) special_pool.push_back(TileType::kLava);

  for (int i = 0; i < Constants::kSpecialTileCount; i++) {
    for (int attempt = 0; attempt < Constants::kMaxSpawnAttempts; attempt++) {
      int x = Constants::kSpawnMargin +
               rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
      int y = Constants::kSpawnMargin +
               rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
      if (current_room_->GetTile(x, y).GetType() == TileType::kFloor) {
        TileType t = special_pool[rand() % special_pool.size()];
        current_room_->GetTile(x, y).SetType(t);
        break;
      }
    }
  }

  for (int attempt = 0; attempt < Constants::kMaxSpawnAttempts; attempt++) {
    int x = Constants::kSpawnMargin +
            rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
    int y = Constants::kSpawnMargin +
            rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
    if (current_room_->GetTile(x, y).GetType() == TileType::kFloor) {
      current_room_->GetTile(x, y).SetType(TileType::kKey);
      break;
    }
  }

  for (int attempt = 0; attempt < Constants::kMaxSpawnAttempts; attempt++) {
    int x = Constants::kSpawnMargin +
            rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
    int y = Constants::kSpawnMargin +
            rand() % (Constants::kFieldSize - Constants::kSpawnMargin * 2);
    if (IsTileFree(x, y)) {
      current_room_->AddDestructible(
          std::make_unique<DroppableChest>(x, y, CreateRandomItem()));
      break;
    }
  }
}

bool GameState::HandleInput(const SDL_KeyboardEvent& key) {
#ifndef __EMSCRIPTEN__
  // В браузере Escape зарезервирован под выход из fullscreen —
  // закрытие игры через эту клавишу там не имеет смысла.
  if (key.keysym.sym == SDLK_ESCAPE) return true;
#endif
  if (key.keysym.sym == SDLK_TAB) {
    Inventory& inv = player_.GetInventory();
    if (inv.GetItemCount() > 0) {
      int next = (inv.GetActiveIndex() + 1) % inv.GetItemCount();
      inv.SetActiveIndex(next);
    }
    return false;
  }
  if (status_ != GameStatus::kPlaying) return false;

  Action action = Action::kWait;
  switch (key.keysym.sym) {
    case SDLK_w:
      action = Action::kMoveUp;
      break;
    case SDLK_s:
      action = Action::kMoveDown;
      break;
    case SDLK_a:
      action = Action::kMoveLeft;
      break;
    case SDLK_d:
      action = Action::kMoveRight;
      break;
    case SDLK_q:
      action = Action::kMoveUpLeft;
      break;
    case SDLK_e:
      action = Action::kMoveUpRight;
      break;
    case SDLK_z:
      action = Action::kMoveDownLeft;
      break;
    case SDLK_c:
      action = Action::kMoveDownRight;
      break;
    case SDLK_x:
      action = Action::kUseItem;
      break;
    case SDLK_1:
      action = Action::kUseAbility1;
      break;
    case SDLK_2:
      action = Action::kUseAbility2;
      break;
    case SDLK_3:
      action = Action::kUseAbility3;
      break;
    case SDLK_4:
      action = Action::kUseAbility4;
      break;
    case SDLK_5:
      action = Action::kUseAbility5;
      break;
    default:
      break;
  }
  ProcessAction(action);
  return false;
}

bool GameState::Run(SDL_Renderer* sdl_renderer) {
  Renderer renderer(sdl_renderer, "assets/cour.ttf");
  const float kFadeSpeed = 6.f;   // единиц alpha за фрейм
  const float kHoldFrames = 60.f; // кол-во фреймов задержки
  float hold_timer = 0.f;

  bool window_open = true;
  while (window_open) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) window_open = false;
      if (event.type == SDL_KEYDOWN) {
        const SDL_KeyboardEvent& key = event.key;
        if (status_ == GameStatus::kDefeat && key.keysym.sym == SDLK_r) {
          transition_ = {true, TransitionPhase::kFadeOut, 0.f,
                        current_circle_};
          pending_restart_level_ = true;
        } else if (status_ == GameStatus::kVictory &&
                  key.keysym.sym == SDLK_r) {
          restart_requested_ = true;
        } else if (!transition_.active && HandleInput(key)) {
          window_open = false;
        }
      }
    }
    if (restart_requested_) break;

    SDL_SetRenderDrawColor(sdl_renderer, 20, 20, 20, 255);
    SDL_RenderClear(sdl_renderer);
    renderer.Draw(*this);

    if (transition_.active) {
      if (transition_.phase == TransitionPhase::kFadeOut) {
        transition_.alpha += kFadeSpeed;
        if (transition_.alpha >= 255.f) {
          transition_.alpha = 255.f;
          transition_.phase = TransitionPhase::kHold;
          hold_timer = 0.f;
        }
      } else if (transition_.phase == TransitionPhase::kHold) {
        if (hold_timer == 0.f && pending_next_level_) {
          NextLevel();
          pending_next_level_ = false;
        } else if (hold_timer == 0.f && pending_restart_level_) {
          RestartLevel();
          pending_restart_level_ = false;
        }
        hold_timer++;
        if (hold_timer >= kHoldFrames)
          transition_.phase = TransitionPhase::kFadeIn;
      } else {
        transition_.alpha -= kFadeSpeed;
        if (transition_.alpha <= 0.f) {
          transition_.alpha = 0.f;
          transition_.active = false;
        }
      }
      renderer.DrawTransition(static_cast<int>(transition_.alpha),
                              transition_.target_circle);
    }

    SDL_RenderPresent(sdl_renderer);
#ifdef __EMSCRIPTEN__
    emscripten_sleep(1000 / Constants::kFramerateLimit);
#else
    SDL_Delay(1000 / Constants::kFramerateLimit);
#endif
  }
  return restart_requested_;
}

bool GameState::IsTileFree(int x, int y) const {
  if (x == player_.GetX() && y == player_.GetY()) return false;
  const Tile& tile = current_room_->GetTile(x, y);
  if (tile.GetType() != TileType::kFloor) return false;
  for (const auto& e : current_room_->GetEnemies())
    if (e->IsAlive() && e->GetX() == x && e->GetY() == y) return false;
  for (const auto& d : current_room_->GetDestructibles())
    if (d->IsAlive() && d->GetX() == x && d->GetY() == y) return false;
  if (current_room_->HasItemAt(x, y)) return false;
  return true;
}

std::unique_ptr<Item> GameState::CreateItemFromData(const ItemData& data) {
  if (data.type == "Weapon") {
    return std::make_unique<Weapon>(data.id, data.name,
                                    0,
                                    static_cast<int>(data.effect_value2),
                                    static_cast<int>(data.effect_value),
                                    0.f);
  }

  EffectType effect = EffectType::kCool;
  if (data.effect_type == "Heat")     effect = EffectType::kHeat;
  else if (data.effect_type == "Heal")    effect = EffectType::kHeal;
  else if (data.effect_type == "Empower") effect = EffectType::kEmpower;

  return std::make_unique<Consumable>(data.id, data.name, effect,
                                      static_cast<int>(data.effect_value));
}

std::unique_ptr<Item> GameState::CreateRandomItem() {
  std::vector<const ItemData*> pool;
  for (const auto& data : database_.GetAllItems()) {
    bool used = false;
    for (const auto& id : used_item_ids_)
      if (id == data.id) { used = true; break; }
    if (!used) pool.push_back(&data);
  }
  if (pool.empty()) return nullptr;

  const ItemData& data = *pool[rand() % pool.size()];
  used_item_ids_.push_back(data.id);
  return CreateItemFromData(data);
}

void GameState::SnapshotInventory() {
  inventory_snapshot_ids_.clear();
  const Inventory& inv = player_.GetInventory();
  for (int i = 0; i < inv.GetItemCount(); i++) {
    Item* item = inv.GetItem(i);
    if (item) inventory_snapshot_ids_.push_back(item->GetItemId());
  }
}

void GameState::RestoreInventory() {
  Inventory& inv = player_.GetInventory();
  inv.Clear();
  for (const auto& id : inventory_snapshot_ids_) {
    const ItemData* data = database_.GetItemData(id);
    if (data) inv.AddItem(CreateItemFromData(*data));
  }
}

void GameState::NextLevel() {
  current_circle_++;
  current_room_ = std::make_unique<Room>(
      Constants::kFieldSize, Constants::kFieldSize, RoomType::kCombat);
  turn_system_.Reset();
  level_generator_.Generate(current_circle_, database_);
  ApplyGeneratedRooms();
  SpawnEnemies();
  player_.SnapshotProgress();
  SnapshotInventory();
}

void GameState::RestartLevel() {
  current_room_ = std::make_unique<Room>(
      Constants::kFieldSize, Constants::kFieldSize, RoomType::kCombat);
  turn_system_.Reset();
  player_.RestoreProgress();
  RestoreInventory();
  status_ = GameStatus::kPlaying;
  level_generator_.Generate(current_circle_, database_);
  ApplyGeneratedRooms();
  SpawnEnemies();
}

const LevelTransition& GameState::GetTransition() const { return transition_; }
