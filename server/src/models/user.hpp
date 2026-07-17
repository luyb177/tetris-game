#pragma once

#include <string>

// 已认证用户在业务层之间传递的最小信息。
struct User {
    long long id{};
    std::string nickname;
};
