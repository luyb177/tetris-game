#pragma once

#include "auth/session.hpp"
#include "repositories/score_repository.hpp"
#include "services/api_result.hpp"

#include <string>

// 成绩业务层：校验登录态、保存单局成绩、组装排行榜数据。
class ScoreService {
public:
    ScoreService(ScoreRepository& scores, SessionStore& sessions);

    ApiResult save(const std::string& token, int score, int level, int layers);
    ApiResult ranking();

private:
    ScoreRepository& scores_;
    SessionStore& sessions_;
};
