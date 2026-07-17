#pragma once

#include <string>

// 排行榜对前端公开的数据结构。
struct RankingEntry {
    std::string nickname;
    int score{};
    int level{};
    int layers{};
};
