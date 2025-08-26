#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

using Callback = std::function<void(const HttpResponsePtr&)>;

namespace Controllers
{

class AuthController final : public HttpController<AuthController>
{
public:
    static void registerUser(const HttpRequestPtr& req, Callback&& callback);
    static void login(const HttpRequestPtr& req, Callback&& callback);
    static void refresh(const HttpRequestPtr& req, Callback&& callback);

public:
    METHOD_LIST_BEGIN

        ADD_METHOD_TO(registerUser, "/auth/register", Post);
        ADD_METHOD_TO(login, "/auth/login", Post);
        ADD_METHOD_TO(refresh, "/auth/refresh", Post);

    METHOD_LIST_END

private:
    static bool validateUser(const std::shared_ptr<Json::Value>& json);
};

}
