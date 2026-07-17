#pragma once

#include <string>
#include <unordered_map>

// 读取 app.yaml 后保存的扁平化配置。
struct Config {
    std::unordered_map<std::string, std::string> values;
};

Config loadConfig(const std::string& path);
std::string configValue(const Config& config,
                        const std::string& key,
                        const std::string& fallback = {});

// 当前项目使用的轻量级请求字段解析和身份凭证生成工具。
std::string jsonField(const std::string& body, const std::string& key);
std::string hashPassword(const std::string& password);
std::string generateCode();
std::string generateToken();
