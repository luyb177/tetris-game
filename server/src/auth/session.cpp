#include "auth/session.hpp"

void SessionStore::add(const std::string& token, long long userId) {
    // 登录或注册成功后保存 Token 与用户 ID 的映射。
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[token] = userId;
}

bool SessionStore::find(const std::string& token, long long& userId) const {
    // 成绩提交时验证 Token；互斥锁保证并发请求读取会话表安全。
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = sessions_.find(token);
    if (found == sessions_.end()) return false;
    userId = found->second;
    return true;
}
