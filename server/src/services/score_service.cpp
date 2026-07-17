#include "services/score_service.hpp"

ScoreService::ScoreService(ScoreRepository& scores, SessionStore& sessions)
    : scores_(scores), sessions_(sessions) {}

ApiResult ScoreService::save(const std::string& token, int score, int level, int layers) {
    long long userId = 0;
    if (token.empty() || !sessions_.find(token, userId)) {
        return {401, jsonObject({{"error", "invalid token"}})};
    }
    if (score < 0 || level < 1 || layers < 0) {
        return {400, jsonObject({{"error", "invalid score data"}})};
    }
    scores_.create(userId, score, level, layers);
    return {201, jsonObject({{"message", "score saved"}})};
}

ApiResult ScoreService::ranking() {
    Json::Value items(Json::arrayValue);
    for (const auto& entry : scores_.findTopRanking(20)) {
        items.append(jsonObject({{"nickname", entry.nickname}, {"score", entry.score},
                                 {"level", entry.level}, {"layers", entry.layers}}));
    }
    return {200, jsonObject({{"items", items}})};
}
