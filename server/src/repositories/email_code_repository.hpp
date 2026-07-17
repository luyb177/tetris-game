#pragma once

#include "data/database.hpp"

#include <string>

// 验证码表访问层：保存和核销验证码。
class EmailCodeRepository {
public:
    explicit EmailCodeRepository(Database& database);

    void create(const std::string& email, const std::string& code);
    bool consumeValid(const std::string& email, const std::string& code);

private:
    Database& database_;
};
