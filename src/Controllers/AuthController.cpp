#include "Controllers/AuthController.hpp"

#include "Utils/JwtUtils.hpp"

#include "Models/UserKeys.hpp"
#include "Models/Users.hpp"

namespace Models = drogon_model::serega;

namespace Controllers
{

void AuthController::registerUser(const HttpRequestPtr& req, Callback&& callback)
{
    const auto request = req->getJsonObject();

    if(!request || !request->isMember("username") || !request->isMember("password")
        || !request->isMember("identity_key") || !request->isMember("pre_key"))
    {
        callback(HttpResponse::newHttpResponse(k400BadRequest, {}));
        return;
    }

    static auto users = orm::Mapper<Models::Users>(app().getDbClient());
    static auto keys = orm::Mapper<Models::UserKeys>(app().getDbClient());

    try
    {
        if(!users.findBy({ "username", (*request)["username"].asString() }).empty())
            throw std::runtime_error("User already exists");

        Models::Users user;
        user.setUsername((*request)["username"].asString());
        // There is a postgres trigger that will safely hash the plain password before inserting
        user.setPasswordHash((*request)["password"].asString());

        users.insert(user);

        Models::UserKeys userKeys;
        userKeys.setUserId(*user.getId());
        userKeys.setIdentityKey(utils::base64Decode((*request)["identity_key"].asString()));
        userKeys.setPreKey(utils::base64Decode((*request)["pre_key"].asString()));

        keys.insert(userKeys);

        (*request)["user_id"] = *user.getId();
    }
    catch(...)
    {
        callback(HttpResponse::newHttpResponse(k409Conflict, {}));
        return;
    }

    login(req, std::move(callback));
}

void AuthController::login(const HttpRequestPtr& req, Callback&& callback)
{
    const auto request = req->getJsonObject();

    if(!request || !request->isMember("username") || !request->isMember("password"))
    {
        callback(HttpResponse::newHttpResponse(k400BadRequest, {}));
        return;
    }

    if(validateUser(request))
    {
        const auto userId = (*request)["user_id"].asInt();
        const auto username = (*request)["username"].asString();
        
        const auto accessToken = Utils::makeAccessToken(userId, username);
        const auto refreshToken = Utils::makeRefreshToken(userId, username);

        Json::Value json;
        json["access_token"] = accessToken;
        json["refresh_token"] = refreshToken;
        json["expires_in"] = 3600;

        const auto response = HttpResponse::newHttpJsonResponse(json);

        callback(response);
    }
    else
    {
        callback(HttpResponse::newHttpResponse(k401Unauthorized, {}));
    }
}

void AuthController::refresh(const HttpRequestPtr& req, Callback&& callback)
{
    static auto refreshSecret = app().getCustomConfig()["jwt"]["refresh_secret"].asString();

    try
    {
        const auto request = req->getJsonObject();
        if(!request)
        {
            callback(HttpResponse::newHttpResponse(k400BadRequest, {}));
            return;
        }

        const auto refreshToken = (*request)["refresh_token"].asString();

        const auto [userId, username] = Utils::verify(refreshToken, refreshSecret);
        const auto accessToken = Utils::makeAccessToken(std::stoi(userId), username);
        const auto newRefreshToken = Utils::makeRefreshToken(std::stoi(userId), username);

        Json::Value json;
        json["access_token"] = accessToken;
        json["refresh_token"] = newRefreshToken;
        json["expires_in"] = 3600;

        callback(HttpResponse::newHttpJsonResponse(json));
    }
    catch(...)
    {
        callback(HttpResponse::newHttpResponse(k401Unauthorized, {}));
    }
}

bool AuthController::validateUser(const std::shared_ptr<Json::Value>& json)
{
    static auto mapper = orm::Mapper<Models::Users>(app().getDbClient());

    try
    {
        // An exception will be thrown if the number of rows != 1
        const auto user = mapper.findOne(
            orm::Criteria{ "username", (*json)["username"].asString() }
            && orm::Criteria{ orm::CustomSql("password_hash = crypt($2, password_hash)"), (*json)["password"].asString() }
        );

        (*json)["user_id"] = *user.getId();

        return true;
    }
    catch(...) {}

    return false;
}

}
