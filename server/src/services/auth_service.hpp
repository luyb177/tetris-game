#pragma once

#include "auth/session.hpp"
#include "config/config.hpp"
#include "repositories/email_code_repository.hpp"
#include "repositories/user_repository.hpp"
#include "services/api_result.hpp"

#include <string>

// 认证业务层：验证码、注册和登录流程集中在这里。
class AuthService {
public:
    AuthService(const Config& config, UserRepository& users,
                EmailCodeRepository& codes, SessionStore& sessions);

    ApiResult sendCode(const std::string& email);
    ApiResult registerUser(const std::string& email, const std::string& nickname,
                           const std::string& password, const std::string& code);
    ApiResult login(const std::string& email, const std::string& password);

private:
    ApiResult createSession(const User& user);

    const Config& config_;
    UserRepository& users_;
    EmailCodeRepository& codes_;
    SessionStore& sessions_;
};
