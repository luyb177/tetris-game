#include "config/config.hpp"

#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>

namespace {

std::string trim(std::string value) {
    // 配置文件允许缩进，因此先去掉键和值两侧的空白字符。
    const auto first = value.find_first_not_of(" \t\r");
    const auto last = value.find_last_not_of(" \t\r");
    if (first == std::string::npos) return {};
    return value.substr(first, last - first + 1);
}

}  // namespace

Config loadConfig(const std::string& path) {
    // 读取 app.yaml 的简单二级结构，例如 mysql.host、email.enabled。
    Config config;
    std::ifstream input(path);
    std::string line;
    std::string section;

    while (std::getline(input, line)) {
        const auto comment = line.find('#');
        if (comment != std::string::npos) line.resize(comment);

        line = trim(line);
        if (line.empty()) continue;

        if (line.back() == ':') {
            section = trim(line.substr(0, line.size() - 1));
            continue;
        }

        const auto colon = line.find(':');
        if (colon == std::string::npos) continue;

        const auto key = trim(line.substr(0, colon));
        auto value = trim(line.substr(colon + 1));
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        config.values[section + "." + key] = value;
    }

    return config;
}

std::string configValue(const Config& config,
                        const std::string& key,
                        const std::string& fallback) {
    const auto found = config.values.find(key);
    return found == config.values.end() ? fallback : found->second;
}

std::string jsonField(const std::string& body, const std::string& key) {
    // 项目请求体结构简单，这里提取字符串或数字字段供路由层使用。
    auto position = body.find('"' + key + '"');
    if (position == std::string::npos) return {};

    position = body.find(':', position);
    position = body.find_first_not_of(" \t\"", position + 1);
    const auto end = body.find_first_of("\",}\r\n", position);
    return body.substr(position, end - position);
}

std::string hashPassword(const std::string& password) {
    // 课程项目使用密码摘要保存数据库值；正式部署应替换为专用密码哈希算法。
    std::hash<std::string> hasher;
    std::ostringstream output;
    output << std::hex << hasher("tetris-v1:" + password);
    return output.str();
}

std::string generateCode() {
    // 生成注册邮件使用的六位数字验证码。
    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(100000, 999999);
    return std::to_string(distribution(generator));
}

std::string generateToken() {
    // 生成后端内存会话使用的随机 Token。
    static std::mt19937_64 generator(std::random_device{}());
    std::ostringstream output;
    output << std::hex << generator() << generator();
    return output.str();
}
