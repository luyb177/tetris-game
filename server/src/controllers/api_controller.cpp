#include "controllers/api_controller.hpp"

namespace {

std::string stringField(const Json::Value& body, const char* key) {
    return body.isMember(key) && body[key].isString() ? body[key].asString() : "";
}

int intField(const Json::Value& body, const char* key, int fallback = 0) {
    return body.isMember(key) && body[key].isInt() ? body[key].asInt() : fallback;
}

}  // namespace

ApiController::ApiController(const Config& config, Database& database,
                             SessionStore& sessions)
    : users_(database),
      codes_(database),
      scores_(database),
      authService_(config, users_, codes_, sessions),
      scoreService_(scores_, sessions) {}

void ApiController::registerRoutes() {
    auto& app = drogon::app();

    app.registerHandler("/api/health",
                        [](const drogon::HttpRequestPtr&,
                           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                            callback(drogon::HttpResponse::newHttpJsonResponse(
                                jsonObject({{"ok", true}, {"service", "tetris-server"}})));
                        },
                        {drogon::Get});

    app.registerHandler("/api/auth/send-code",
                        [this](const drogon::HttpRequestPtr& request,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                            const auto body = readJson(request);
                            callback(makeResponse(authService_.sendCode(
                                stringField(body, "email"))));
                        },
                        {drogon::Post});

    app.registerHandler("/api/auth/register",
                        [this](const drogon::HttpRequestPtr& request,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                            const auto body = readJson(request);
                            callback(makeResponse(authService_.registerUser(
                                stringField(body, "email"), stringField(body, "nickname"),
                                stringField(body, "password"), stringField(body, "code"))));
                        },
                        {drogon::Post});

    app.registerHandler("/api/auth/login",
                        [this](const drogon::HttpRequestPtr& request,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                            const auto body = readJson(request);
                            callback(makeResponse(authService_.login(
                                stringField(body, "email"), stringField(body, "password"))));
                        },
                        {drogon::Post});

    app.registerHandler("/api/scores",
                        [this](const drogon::HttpRequestPtr& request,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                            const auto body = readJson(request);
                            callback(makeResponse(scoreService_.save(
                                bearerToken(request), intField(body, "score"),
                                intField(body, "level", 1), intField(body, "layers"))));
                        },
                        {drogon::Post});

    app.registerHandler("/api/scores/ranking",
                        [this](const drogon::HttpRequestPtr&,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                            callback(makeResponse(scoreService_.ranking()));
                        },
                        {drogon::Get});
}

drogon::HttpResponsePtr ApiController::makeResponse(const ApiResult& result) {
    auto response = drogon::HttpResponse::newHttpJsonResponse(result.body);
    response->setStatusCode(static_cast<drogon::HttpStatusCode>(result.status));
    return response;
}

Json::Value ApiController::readJson(const drogon::HttpRequestPtr& request) {
    const auto json = request->getJsonObject();
    return json ? *json : Json::Value(Json::objectValue);
}

std::string ApiController::bearerToken(const drogon::HttpRequestPtr& request) {
    const auto header = request->getHeader("authorization");
    constexpr std::string_view prefix = "Bearer ";
    if (header.rfind(prefix, 0) != 0) return {};
    return header.substr(prefix.size());
}
