#pragma once

#include "data/database.hpp"
#include "models/user.hpp"

#include <optional>
#include <string>

// 用户表访问层：只处理 users 表，不包含登录和 Token 业务。
class UserRepository {
public:
    explicit UserRepository(Database& database);

    void create(const std::string& email, const std::string& passwordHash,
                const std::string& nickname);
    std::optional<User> findByCredentials(const std::string& email,
                                          const std::string& passwordHash);
    std::optional<User> findByEmail(const std::string& email);

private:
    Database& database_;
};
