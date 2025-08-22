#pragma once
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpResponse.h>

#include <jwt-cpp/jwt.h>

using namespace drogon;
using namespace std::chrono_literals;

namespace Utils
{

inline std::pair<std::string, std::string> verify(const std::string& token, const std::string& secret)
{
    const auto decoded = jwt::decode(token);
    const auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256(secret))
            .with_issuer("auth0");

    verifier.verify(decoded);

    return {
        decoded.get_payload_claim("user_id").as_string(),
        decoded.get_payload_claim("username").as_string()
    };
}

inline std::string makeAccessToken(const int id, const std::string& username)
{
    static auto accessSecret = app().getCustomConfig()["jwt"]["access_secret"].asString();

    auto token = jwt::create()
        .set_issuer("auth0")
        .set_type("JWS")
        .set_expires_in(3600s) // 1 hour
        .set_payload_claim(
            "user_id",
            jwt::claim(std::to_string(id))
        )
        .set_payload_claim(
            "username",
            jwt::claim(username)
        )
        .sign(jwt::algorithm::hs256(accessSecret));

    return token;
}

inline std::string makeRefreshToken(const int id, const std::string& username)
{
    static auto refreshSecret = app().getCustomConfig()["jwt"]["refresh_secret"].asString();

    auto token = jwt::create()
        .set_issuer("auth0")
        .set_type("JWS")
        .set_expires_in(604800s) // 7 days
        .set_payload_claim(
            "user_id",
            jwt::claim(std::to_string(id))
        )
        .set_payload_claim(
            "username",
            jwt::claim(username)
        )
        .sign(jwt::algorithm::hs256(refreshSecret));

    return token;
}

}
