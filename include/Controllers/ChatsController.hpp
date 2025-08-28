#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

using Callback = std::function<void(const HttpResponsePtr&)>;

namespace Controllers
{

class ChatsController final : public HttpController<ChatsController>
{
public:
    static void sendMessage(const HttpRequestPtr& req, Callback&& callback, int chatId);
    static void getMessages(const HttpRequestPtr& req, Callback&& callback, int chatId, int limit = 50, int offset = 0);

public:
    METHOD_LIST_BEGIN

        ADD_METHOD_TO(sendMessage, "/chats/{chat_id}/messages", Post, "Filters::JwtFilter");
        ADD_METHOD_TO(getMessages, "/users/{chat_id}/messages", Get, "Filters::JwtFilter");

    METHOD_LIST_END
};

}
