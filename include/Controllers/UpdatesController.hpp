#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

using Callback = std::function<void(const HttpResponsePtr&)>;

namespace Controllers
{

class UpdatesController final : public HttpController<UpdatesController>
{
public:
    static void getUpdates(const HttpRequestPtr& req, Callback&& callback, int limit = 50, int offset = 0);

public:
    METHOD_LIST_BEGIN

        ADD_METHOD_TO(getUpdates, "/updates?limit={limit}&offset={offset}", Get, "Filters::JwtFilter");

    METHOD_LIST_END
};

}
