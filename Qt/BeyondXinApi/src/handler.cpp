#include "handler.h"

#include <chrono>
#include <thread>

#include "EventLoop.h"
#include "axios.h"
#include "hbase.h"
#include "hfile.h"
#include "hstring.h"
#include "htime.h"
#include "requests.h"

std::string Handler::m_ClientId = "";
std::string Handler::m_ClientSecret = "";

int Handler::Preprocessor(HttpRequest * req, HttpResponse * resp)
{
#if REDIRECT_HTTP_TO_HTTPS
    // 301
    if (req->scheme == "http") {
        std::string location = hv::asprintf("https://%s:%d%s", req->host.c_str(),
                                            8443, req->path.c_str());
        return resp->Redirect(location, HTTP_STATUS_MOVED_PERMANENTLY);
    }
#endif

    req->ParseBody();
    resp->content_type = APPLICATION_JSON;
    return HTTP_STATUS_NEXT;
}

int Handler::GetPing(HttpRequest * req, HttpResponse * resp)
{
    return resp->String("pong");
}

int Handler::Postprocessor(HttpRequest * req, HttpResponse * resp)
{
    return resp->status_code;
}

int Handler::GetGithubAuthorize(const HttpContextPtr & ctx)
{
    std::cout << "get GithubAuthorize" << std::endl;

    HttpRequest req;
    req.method = HTTP_POST;
    req.url = "https://github.com/login/oauth/access_token";
    req.headers["accept"] = "application/json";
    req.body = "";
    req.timeout = 3;
    req.retry_count = 2;
    req.retry_delay = 1500;
    req.query_params["client_id"] = m_ClientId;
    req.query_params["client_secret"] = m_ClientSecret;
    req.query_params["code"] = ctx->param("code", "");

    HttpResponse resp;
    hv::HttpClient syncClient;
    int ret = syncClient.send(&req, &resp);
    if (ret != 0) {
        std::cout << "request failed! " << ret << std::endl;
        return 403;
    }
    hv::Json rJson = resp.GetJson();
    if (rJson["access_token"].empty()) {
        std::cout << "query failed !" << std::endl;
        std::cout << "error: " << resp.body.c_str() << std::endl;
        std::cout << "error: " << rJson["error"] << std::endl;
        std::cout << "error_description: " << rJson["error_description"]
                  << std::endl;
        ctx->set("msg", rJson["error_description"]);
        response_status(ctx, 0, "OK");
        return 401;
    }
    std::string token = rJson["access_token"];
    ctx->setContentType("application/json");
    ctx->set("token", token);
    response_status(ctx, 0, "OK");
    return 200;
}
