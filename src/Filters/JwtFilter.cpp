#include "Filters/JwtFilter.hpp"

#include "Utils/JwtUtils.hpp"

namespace Filters
{

void JwtFilter::doFilter(
    const HttpRequestPtr& req,
    FilterCallback&& fcb,
    FilterChainCallback&& fccb
)
{
    static auto accessSecret = app().getCustomConfig()["jwt"]["access_secret"].asString();

    const auto header = req->getHeader("Authorization");

    if(header.empty())
    {
        const auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k401Unauthorized);

        fcb(response);

        return;
    }

    try
    {
        const auto [userId, username] = Utils::verify(header.substr(7), accessSecret);

        req->setParameter("user_id", userId);
        req->setParameter("username", username);

        fccb();
    }
    catch(...)
    {
        const auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k401Unauthorized);

        fcb(response);
    }
}

}
