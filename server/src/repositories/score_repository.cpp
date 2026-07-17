#include "repositories/score_repository.hpp"

ScoreRepository::ScoreRepository(Database& database) : database_(database) {}

void ScoreRepository::create(long long userId, int score, int level, int layers) {
    database_.execute(
        "INSERT INTO scores(user_id,score,level,layers) VALUES(" +
        std::to_string(userId) + "," + std::to_string(score) + "," +
        std::to_string(level) + "," + std::to_string(layers) + ")");
}

std::vector<RankingEntry> ScoreRepository::findTopRanking(int limit) {
    const auto rows = database_.query(
        "SELECT u.nickname,s.score,s.level,s.layers FROM scores s JOIN users u ON "
        "u.id=s.user_id WHERE NOT EXISTS (SELECT 1 FROM scores s2 WHERE "
        "s2.user_id=s.user_id AND (s2.score>s.score OR (s2.score=s.score AND "
        "s2.id>s.id))) ORDER BY s.score DESC LIMIT " + std::to_string(limit));

    std::vector<RankingEntry> entries;
    entries.reserve(rows.size());
    for (const auto& row : rows) {
        entries.push_back({row[0], std::stoi(row[1]), std::stoi(row[2]),
                           std::stoi(row[3])});
    }
    return entries;
}
