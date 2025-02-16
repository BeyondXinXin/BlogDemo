#include "router.h"

#include "handler.h"
#include "hasync.h"
#include "hthread.h"
#include "requests.h"

void Router::Register(hv::HttpService& router)
{
	router.preprocessor = Handler::Preprocessor;
	router.postprocessor = Handler::Postprocessor;

	router.GET("/ping", Handler::GetPing);
	router.POST("/store", Handler::PostStore);
	router.GET("/retrieve", Handler::GetRetrieve);
	router.Delete("/delete", Handler::DeleteData);

	router.GET("/paths",
	           [&router](HttpRequest* req, HttpResponse* resp) {
		return resp->Json(router.Paths());
	});
}
