#pragma once
#include <array>
#include <vector>

struct Cell { int x{}; int y{}; };
using Shape = std::array<Cell, 4>;

enum class PieceType { I, O, T, S, Z, J, L };

class Board {
public:
    static constexpr int Width = 10;
    static constexpr int Height = 20;
    Board();
    void reset();
    bool canPlace(const Shape& shape, int x, int y) const;
    void lock(const Shape& shape, int x, int y, PieceType type);
    int clearLines();
    bool occupied(int x, int y) const;
    PieceType at(int x, int y) const;
private:
    std::array<std::array<int, Width>, Height> cells_{};
};

Shape baseShape(PieceType type);
PieceType randomPiece();
Shape rotate(const Shape& shape);

