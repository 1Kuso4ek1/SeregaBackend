#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

using Callback = std::function<void(const HttpResponsePtr&)>;

namespace Controllers
{

class LoginController final : public HttpController<LoginController>
{
public:
    void registerUser(const HttpRequestPtr& req, Callback&& callback);
    void login(const HttpRequestPtr& req, Callback&& callback);

public:
    METHOD_LIST_BEGIN

        ADD_METHOD_TO(LoginController::registerUser, "/auth/register", Post);
        ADD_METHOD_TO(LoginController::login, "/auth/login", Post);

    METHOD_LIST_END

private:
    bool validateUser(const std::shared_ptr<Json::Value>& json);
};

}
