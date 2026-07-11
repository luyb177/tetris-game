#include "Board.hpp"
#include <algorithm>
#include <random>

Board::Board() { reset(); }
void Board::reset() { for (auto& row : cells_) row.fill(-1); }
bool Board::canPlace(const Shape& s, int x, int y) const {
    for (auto c : s) {
        int px = x + c.x, py = y + c.y;
        if (px < 0 || px >= Width || py >= Height || (py >= 0 && cells_[py][px] >= 0)) return false;
    }
    return true;
}
void Board::lock(const Shape& s, int x, int y, PieceType type) {
    for (auto c : s) if (y + c.y >= 0) cells_[y + c.y][x + c.x] = static_cast<int>(type);
}
int Board::clearLines() {
    int cleared = 0;
    for (int y = Height - 1; y >= 0; --y) {
        if (std::all_of(cells_[y].begin(), cells_[y].end(), [](int v) { return v >= 0; })) {
            for (int row = y; row > 0; --row) cells_[row] = cells_[row - 1];
            cells_[0].fill(-1); ++cleared; ++y;
        }
    }
    return cleared;
}
bool Board::occupied(int x, int y) const { return cells_[y][x] >= 0; }
PieceType Board::at(int x, int y) const { return static_cast<PieceType>(cells_[y][x]); }

Shape baseShape(PieceType type) {
    switch (type) {
        case PieceType::I: return {{{-1,0},{0,0},{1,0},{2,0}}};
        case PieceType::O: return {{{0,0},{1,0},{0,1},{1,1}}};
        case PieceType::T: return {{{-1,0},{0,0},{1,0},{0,1}}};
        case PieceType::S: return {{{0,0},{1,0},{-1,1},{0,1}}};
        case PieceType::Z: return {{{-1,0},{0,0},{0,1},{1,1}}};
        case PieceType::J: return {{{-1,0},{-1,1},{0,1},{1,1}}};
        case PieceType::L: return {{{1,0},{-1,1},{0,1},{1,1}}};
    }
    return {};
}
PieceType randomPiece() { static std::mt19937 gen(std::random_device{}()); static std::uniform_int_distribution<int> d(0,6); return static_cast<PieceType>(d(gen)); }
Shape rotate(const Shape& s) { Shape r{}; for (int i=0;i<4;++i) r[i] = {-s[i].y, s[i].x}; return r; }

