#ifndef HV_HTTPD_HANDLER_H
#define HV_HTTPD_HANDLER_H

#include "HttpService.h"

class Handler
{
public:
    // preprocessor => middleware -> handlers => postprocessor
    static int Preprocessor(HttpRequest * req, HttpResponse * resp);
    static int Postprocessor(HttpRequest * req, HttpResponse * resp);

    static int GetPing(HttpRequest * req, HttpResponse * resp);

    static int GetGithubAuthorize(const HttpContextPtr & req);

    static std::string m_ClientId;
    static std::string m_ClientSecret;

private:
    static int response_status(HttpResponse * resp, int code = 200,
                               const char * message = NULL)
    {
        if (message == NULL)
            message = http_status_str((enum http_status)code);
        resp->Set("code", code);
        resp->Set("message", message);
        return code;
    }
    static int response_status(const HttpResponseWriterPtr & writer,
                               int code = 200, const char * message = NULL)
    {
        response_status(writer->response.get(), code, message);
        writer->End();
        return code;
    }
    static int response_status(const HttpContextPtr & ctx, int code = 200,
                               const char * message = NULL)
    {
        response_status(ctx->response.get(), code, message);
        ctx->send();
        return code;
    }
};

#endif // HV_HTTPD_HANDLER_H
