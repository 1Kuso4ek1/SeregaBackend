#include "Controllers/UpdatesController.hpp"

#include "Models/Messages.hpp"
#include "Models/Updates.hpp"

namespace Models = drogon_model::serega;

namespace Controllers
{

void UpdatesController::getUpdates(
    const HttpRequestPtr& req, Callback&& callback,
    const int limit, const int offset
)
{
    static auto updates = orm::Mapper<Models::Updates>(app().getDbClient());

    try
    {
        const auto userId = std::stoi(req->getParameter("user_id"));

        auto sendUpdates = [send = std::move(callback)](const auto& userUpdates)
        {
            Json::Value res{ Json::arrayValue };
            for(const auto& update : userUpdates)
                res.append(update.toJson());

            send(HttpResponse::newHttpJsonResponse(res));
        };

        auto waitUpdates = [=, send = std::move(sendUpdates)](this const auto self) -> void
        {
            updates
                .limit(limit).offset(offset)
                .findBy({ "user_id", userId },
                    [send, self](const auto& userUpdates)
                    {
                        if(!userUpdates.empty())
                            send(userUpdates);
                        else
                            app().getLoop()->runAfter(2.0, [self] { self(); });
                    },
                    [send](const auto&) { send(std::vector<Models::Updates>{}); });
        };

        waitUpdates();
    }
    catch(...)
    {
        callback(HttpResponse::newHttpResponse(k500InternalServerError, {}));
    }
}

}
