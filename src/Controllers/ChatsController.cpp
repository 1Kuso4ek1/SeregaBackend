#include "Controllers/ChatsController.hpp"

#include "Models/Messages.hpp"

namespace Models = drogon_model::serega;

namespace Controllers
{

void ChatsController::sendMessage(const HttpRequestPtr& req, Callback&& callback, const int chatId)
{
    const auto json = req->getJsonObject();
    if(!json || !json->isMember("content"))
    {
        callback(HttpResponse::newHttpResponse(k400BadRequest, {}));
        return;
    }

    static auto messages = orm::Mapper<Models::Messages>(app().getDbClient());

    try
    {
        const auto userId = std::stoi(req->getParameter("user_id"));

        Models::Messages message;
        message.setContent(utils::base64Decode((*json)["content"].asString()));
        message.setSenderId(userId);
        message.setReceiverId(chatId);

        messages.insert(message);

        Json::Value res;
        res["message_id"] = *message.getId();

        const auto response = HttpResponse::newHttpJsonResponse(res);
        response->setStatusCode(k201Created);

        callback(response);
    }
    catch(...)
    {
        callback(HttpResponse::newHttpResponse(k404NotFound, {}));
    }
}

void ChatsController::getMessages(
    const HttpRequestPtr& req, Callback&& callback,
    const int chatId, const int limit, const int offset
)
{
    static auto messages = orm::Mapper<Models::Messages>(app().getDbClient());

    try
    {
        const auto userId = std::stoi(req->getParameter("user_id"));

        const auto chatMessages = messages
            .limit(limit).offset(offset)
            .orderBy("created_at")
            .findBy((orm::Criteria{ "sender_id", userId }
                        && orm::Criteria{ "receiver_id", chatId })
                    || (orm::Criteria{ "sender_id", chatId }
                        && orm::Criteria{ "receiver_id", userId }));

        Json::Value res{ Json::arrayValue };
        for(const auto& message : chatMessages)
            res.append(message.toJson());

        callback(HttpResponse::newHttpJsonResponse(res));
    }
    catch(...)
    {
        callback(HttpResponse::newHttpResponse(k404NotFound, {}));
    }
}

}
