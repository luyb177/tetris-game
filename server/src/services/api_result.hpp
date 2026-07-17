#pragma once

#include <json/value.h>

#include <initializer_list>
#include <string>
#include <utility>

// Service 返回的统一结果，Controller 负责转换为 HTTP 响应。
struct ApiResult {
    int status{200};
    Json::Value body;
};

// JsonCpp 不支持对象初始化列表；此工具保持构造响应时的可读性。
inline Json::Value jsonObject(
    std::initializer_list<std::pair<std::string, Json::Value>> fields) {
    Json::Value value(Json::objectValue);
    for (const auto& [key, field] : fields) value[key] = field;
    return value;
}
