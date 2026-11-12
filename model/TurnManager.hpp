#pragma once
enum class Phase { PlayerTurn, EnemyTurn, BattleOver };
class TurnManager {
public:
    Phase phase() const { return phase_; }
    int   turnNumber() const { return turnNumber_; }
    void endPlayerTurn() { phase_ = Phase::EnemyTurn; }
    void endEnemyTurn()  { phase_ = Phase::PlayerTurn; ++turnNumber_; }
    void setBattleOver() { phase_ = Phase::BattleOver; }
private:
    Phase phase_ = Phase::PlayerTurn;
    int   turnNumber_ = 1;
};