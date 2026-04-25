#include "HeatPurge.h"

#include "Player.h"
#include "Room.h"

void HeatPurge::Activate(Player& player, Room& room) {
  if (!IsReady()) return;
  player.AddCoreHeat(-player.GetCoreHeat());  // полный сброс
  current_cooldown_ = cooldown_;
}