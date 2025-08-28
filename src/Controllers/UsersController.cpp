#include "Controllers/UsersController.hpp"

#include "Models/UserKeys.hpp"
#include "Models/Users.hpp"

namespace Models = drogon_model::serega;

namespace Controllers
{

void UsersController::getUser(const HttpRequestPtr& req, Callback&& callback, const int id)
{
    static auto users = orm::Mapper<Models::Users>(app().getDbClient());
    static auto keys = orm::Mapper<Models::UserKeys>(app().getDbClient());

    try
    {
        const auto user = users.findOne({ "id", id });
        const auto userKeys = user.getKeys(app().getDbClient());

        if(userKeys.empty())
            throw std::runtime_error("User has no keys");

        Json::Value json;
        json["id"] = *user.getId();
        json["username"] = *user.getUsername();
        json["identity_key"] = utils::base64Encode(userKeys.front().getIdentityKey()->data());
        json["pre_key"] = utils::base64Encode(userKeys.front().getPreKey()->data());

        callback(HttpResponse::newHttpJsonResponse(json));
    }
    catch(...)
    {
        callback(HttpResponse::newHttpResponse(k404NotFound, {}));
    }
}

void UsersController::getUserByUsername(const HttpRequestPtr& req, Callback&& callback, const std::string& username)
{
    static auto users = orm::Mapper<Models::Users>(app().getDbClient());

    try
    {
        const auto user = users.findOne({ "username", username });
        getUser(req, std::move(callback), *user.getId());
    }
    catch(...)
    {
        callback(HttpResponse::newHttpResponse(k404NotFound, {}));
    }
}

void UsersController::getMe(const HttpRequestPtr& req, Callback&& callback)
{
    getUser(req, std::move(callback), std::stoull(req->getParameter("user_id")));
}

}
