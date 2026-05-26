#include "IAbilityObserver.h"

#include "Ability.h"
#include "Player.h"

void AbilityLogger::OnAbilityActivated(const Ability& ability, Player& player,
                                       Room& /*room*/) {
  // Записываем событие: "Уровень X использовал <название абилки>"
  log_.push_back("Lvl " + std::to_string(player.GetLevel()) +
                 " used: " + ability.GetName());
}
