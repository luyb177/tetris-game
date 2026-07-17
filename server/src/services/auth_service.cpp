#include "services/auth_service.hpp"

#include "services/mail.hpp"

AuthService::AuthService(const Config& config, UserRepository& users,
                         EmailCodeRepository& codes, SessionStore& sessions)
    : config_(config), users_(users), codes_(codes), sessions_(sessions) {}

ApiResult AuthService::sendCode(const std::string& email) {
    if (email.empty()) return {400, jsonObject({{"error", "email is required"}})};

    const auto code = generateCode();
    codes_.create(email, code);
    if (!sendVerificationMail(config_, email, code)) {
        return {500, jsonObject({{"error", "email delivery failed"}})};
    }
    return {200, jsonObject({{"message", "verification code sent"}, {"retryAfter", 60}})};
}

ApiResult AuthService::registerUser(const std::string& email,
                                    const std::string& nickname,
                                    const std::string& password,
                                    const std::string& code) {
    if (email.empty() || nickname.empty() || password.empty() || code.empty()) {
        return {400, jsonObject({{"error", "email, nickname, password and code are required"}})};
    }
    if (!codes_.consumeValid(email, code)) {
        return {400, jsonObject({{"error", "invalid verification code"}})};
    }

    if (users_.findByEmail(email)) {
        return {409, jsonObject({{"error", "email is already registered"}})};
    }

    users_.create(email, hashPassword(password), nickname);
    const auto user = users_.findByEmail(email);
    if (!user) return {500, jsonObject({{"error", "user creation failed"}})};
    return createSession(*user);
}

ApiResult AuthService::login(const std::string& email, const std::string& password) {
    const auto user = users_.findByCredentials(email, hashPassword(password));
    if (!user) return {401, jsonObject({{"error", "invalid credentials"}})};
    return createSession(*user);
}

ApiResult AuthService::createSession(const User& user) {
    const auto token = generateToken();
    sessions_.add(token, user.id);
    return {200, jsonObject({{"token", token}, {"nickname", user.nickname}})};
}
