#pragma once

#include <string>
#include <vector>

class Ability;
class Player;
class Room;

// Observer (наблюдатель): реагирует на использование абилок
class IAbilityObserver {
 public:
  virtual ~IAbilityObserver() = default;
  virtual void OnAbilityActivated(const Ability& ability, Player& player,
                                  Room& room) = 0;
};

// Хранит историю использованных абилок за сессию
class AbilityLogger : public IAbilityObserver {
 public:
  void OnAbilityActivated(const Ability& ability, Player& player,
                          Room& room) override;

  const std::vector<std::string>& GetLog() const { return log_; }
  void Clear() { log_.clear(); }

 private:
  std::vector<std::string> log_;
};
