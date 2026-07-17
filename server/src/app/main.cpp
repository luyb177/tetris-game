#include "auth/session.hpp"
#include "config/config.hpp"
#include "controllers/api_controller.hpp"
#include "data/database.hpp"
#include "services/api_result.hpp"

#include <curl/curl.h>
#include <drogon/drogon.h>

#include <iostream>
#include <string>

// 应用启动入口：初始化配置、数据库、控制器并启动 Drogon 事件循环。
int main(int argc, char** argv) {
    try {
        const std::string configPath =
            argc > 1 ? argv[1] : "server/config/app.yaml";
        const Config config = loadConfig(configPath);
        curl_global_init(CURL_GLOBAL_DEFAULT);
        Database database(config);
        SessionStore sessions;
        ApiController controller(config, database, sessions);

        controller.registerRoutes();

        // 所有 API 响应统一添加 CORS 头，前端可独立部署到其他端口。
        drogon::app().registerPreSendingAdvice(
            [](const drogon::HttpRequestPtr&, const drogon::HttpResponsePtr& response) {
                response->addHeader("Access-Control-Allow-Origin", "*");
                response->addHeader("Access-Control-Allow-Headers",
                                    "Content-Type, Authorization");
                response->addHeader("Access-Control-Allow-Methods",
                                    "GET, POST, OPTIONS");
            });

        // JSON 请求会触发浏览器预检；在进入业务路由前直接响应 OPTIONS。
        drogon::app().registerPreRoutingAdvice(
            [](const drogon::HttpRequestPtr& request, drogon::AdviceCallback&& callback,
               drogon::AdviceChainCallback&& chainCallback) {
                if (request->method() == drogon::Options) {
                    callback(drogon::HttpResponse::newHttpResponse());
                    return;
                }
                chainCallback();
            });

        // 防止数据库或邮件服务异常直接中断请求连接，统一返回 JSON 错误。
        drogon::app().setExceptionHandler(
            [](const std::exception& error, const drogon::HttpRequestPtr&,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                auto body = jsonObject({{"error", "internal server error"}});
                auto response = drogon::HttpResponse::newHttpJsonResponse(body);
                response->setStatusCode(drogon::k500InternalServerError);
                std::cerr << "[REQUEST] " << error.what() << '\n';
                callback(response);
            });

        const auto port = static_cast<uint16_t>(
            std::stoi(configValue(config, "server.port", "8080")));
        drogon::app().addListener("0.0.0.0", port);
        std::cout << "Tetris Drogon server listening on port " << port << '\n';
        drogon::app().run();
    } catch (const std::exception& error) {
        std::cerr << "[SERVER] " << error.what() << '\n';
        return 1;
    }
}
