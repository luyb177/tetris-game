#pragma once

#include "auth/session.hpp"
#include "config/config.hpp"
#include "data/database.hpp"
#include "repositories/email_code_repository.hpp"
#include "repositories/score_repository.hpp"
#include "repositories/user_repository.hpp"
#include "services/auth_service.hpp"
#include "services/score_service.hpp"

#include <drogon/drogon.h>

// HTTP 控制器：只负责 HTTP 请求、JSON 解析和调用对应 Service。
class ApiController {
public:
    ApiController(const Config& config, Database& database,
                  SessionStore& sessions);

    void registerRoutes();

private:
    static drogon::HttpResponsePtr makeResponse(const ApiResult& result);
    static Json::Value readJson(const drogon::HttpRequestPtr& request);
    static std::string bearerToken(const drogon::HttpRequestPtr& request);

    UserRepository users_;
    EmailCodeRepository codes_;
    ScoreRepository scores_;
    AuthService authService_;
    ScoreService scoreService_;
};
