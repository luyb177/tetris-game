#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

// 会话层：保存 Token 与用户 ID 的临时映射。
class SessionStore {
public:
    void add(const std::string& token, long long userId);
    bool find(const std::string& token, long long& userId) const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, long long> sessions_;
};
