#include "data/database.hpp"

#include <stdexcept>

Database::Database(const Config& config) {
    // 后端启动时建立唯一数据库连接，并统一使用 utf8mb4 字符集。
    connection_ = mysql_init(nullptr);
    if (!connection_) throw std::runtime_error("mysql_init failed");

    mysql_options(connection_, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    const auto connected = mysql_real_connect(
        connection_,
        configValue(config, "mysql.host", "127.0.0.1").c_str(),
        configValue(config, "mysql.user", "root").c_str(),
        configValue(config, "mysql.password", "").c_str(),
        configValue(config, "mysql.database", "tetris").c_str(),
        std::stoi(configValue(config, "mysql.port", "3306")),
        nullptr,
        0);

    if (!connected) throw std::runtime_error(mysql_error(connection_));
}

Database::~Database() {
    // 进程退出时释放 MySQL 连接资源。
    if (connection_) mysql_close(connection_);
}

void Database::execute(const std::string& sql) {
    // 执行 INSERT、UPDATE 等不需要返回结果集的 SQL。
    std::lock_guard<std::mutex> lock(mutex_);
    if (mysql_query(connection_, sql.c_str())) {
        throw std::runtime_error(mysql_error(connection_));
    }
}

std::string Database::escape(const std::string& value) {
    // 对外部输入进行 MySQL 字符串转义，避免直接拼接造成注入风险。
    std::lock_guard<std::mutex> lock(mutex_);
    std::string escaped(value.size() * 2 + 1, '\0');
    const auto length = mysql_real_escape_string(
        connection_, escaped.data(), value.c_str(), value.size());
    escaped.resize(length);
    return escaped;
}

std::vector<std::vector<std::string>> Database::query(const std::string& sql) {
    // 执行 SELECT 并将结果转换为便于路由层处理的二维字符串数组。
    std::lock_guard<std::mutex> lock(mutex_);
    if (mysql_query(connection_, sql.c_str())) {
        throw std::runtime_error(mysql_error(connection_));
    }
    MYSQL_RES* result = mysql_store_result(connection_);
    std::vector<std::vector<std::string>> rows;
    if (!result) return rows;

    MYSQL_ROW row;
    const auto fieldCount = mysql_num_fields(result);
    while ((row = mysql_fetch_row(result))) {
        std::vector<std::string> values;
        for (unsigned i = 0; i < fieldCount; ++i) {
            values.emplace_back(row[i] ? row[i] : "");
        }
        rows.push_back(std::move(values));
    }

    mysql_free_result(result);
    return rows;
}
