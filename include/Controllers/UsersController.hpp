#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

using Callback = std::function<void(const HttpResponsePtr&)>;

namespace Controllers
{

class UsersController final : public HttpController<UsersController>
{
public:
    static void getUser(const HttpRequestPtr& req, Callback&& callback, int id);
    static void getUserByUsername(const HttpRequestPtr& req, Callback&& callback, const std::string& username);
    static void getMe(const HttpRequestPtr& req, Callback&& callback);

public:
    METHOD_LIST_BEGIN

        ADD_METHOD_TO(getUser, "/users/{id}", Get, "Filters::JwtFilter");
        ADD_METHOD_TO(getUserByUsername, "/users/name/{username}", Get, "Filters::JwtFilter");
        ADD_METHOD_TO(getMe, "/users/me", Get, "Filters::JwtFilter");

    METHOD_LIST_END
};

}
