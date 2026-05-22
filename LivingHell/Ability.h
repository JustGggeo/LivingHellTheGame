#pragma once
#include <string>

class Player;
class Room;

class Ability {
 public:
  Ability(const std::string& name, int cooldown);
  virtual ~Ability() = default;

  virtual void Activate(Player& player, Room& room) = 0;

  bool IsReady() const;
  void OnTurnPassed();  // вызывать каждый ход в TurnSystem
  const std::string& GetName() const;

 protected:
  std::string name_;
  int cooldown_;  // сколько ходов ждать
  int current_cooldown_;
};
