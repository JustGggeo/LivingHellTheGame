#include "Ability.h"

Ability::Ability(const std::string& name, int cooldown)
    : name_(name), cooldown_(cooldown), current_cooldown_(0) {}

bool Ability::IsReady() const { return current_cooldown_ == 0; }

void Ability::OnTurnPassed() {
  if (current_cooldown_ > 0) current_cooldown_--;
}

const std::string& Ability::GetName() const { return name_; }