#pragma once
#include "Board.hpp"
#include <SFML/Window/Event.hpp>

class Game {
public:
    void update(float dt);
    void handleKey(int key);
    const Board& board() const { return board_; }
    Shape current() const { return current_; }
    PieceType currentType() const { return currentType_; }
    int currentX() const { return x_; }
    int currentY() const { return y_; }
    int score() const { return score_; }
    int lines() const { return lines_; }
    int level() const { return level_; }
    bool gameOver() const { return gameOver_; }
    void restart();
private:
    void spawn(); void drop(); void move(int dx); void rotatePiece();
    Board board_; Shape current_{}; PieceType currentType_{}; int x_{4}, y_{0};
    float timer_{}; int score_{}, lines_{}, level_{1}; bool gameOver_{false};
};

