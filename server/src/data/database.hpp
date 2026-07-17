#pragma once

#include "config/config.hpp"

#include <mysql.h>

#include <mutex>
#include <string>
#include <vector>

// 数据库访问层：集中管理连接、转义、查询和更新操作。
class Database {
public:
    explicit Database(const Config& config);
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    void execute(const std::string& sql);
    std::string escape(const std::string& value);
    std::vector<std::vector<std::string>> query(const std::string& sql);

private:
    // 当前课程项目使用单个 MySQL 连接；Drogon 并发请求通过该锁安全复用连接。
    mutable std::mutex mutex_;
    MYSQL* connection_{};
};
