#pragma once

#include "data/database.hpp"
#include "models/ranking_entry.hpp"

#include <vector>

// 成绩表访问层：写入单局成绩并查询去重排行榜。
class ScoreRepository {
public:
    explicit ScoreRepository(Database& database);

    void create(long long userId, int score, int level, int layers);
    std::vector<RankingEntry> findTopRanking(int limit);

private:
    Database& database_;
};
