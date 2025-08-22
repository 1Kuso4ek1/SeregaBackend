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
        const auto decoded = jwt::decode(header.substr(7));
        const auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256(accessSecret))
            .with_issuer("auth0");

        verifier.verify(decoded);

        req->setParameter("user_id", decoded.get_payload_claim("user_id").as_string());
        req->setParameter("username", decoded.get_payload_claim("username").as_string());

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
