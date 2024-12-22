#include "handler.h"

#include <chrono>
#include <thread>
#include <fstream>
#include <filesystem>

#include "EventLoop.h"
#include "axios.h"
#include "hbase.h"
#include "hfile.h"
#include "hstring.h"
#include "htime.h"
#include "requests.h"


std::string Handler::m_ClientId = "";
std::string Handler::m_ClientSecret = "";

int Handler::Preprocessor(HttpRequest* req, HttpResponse* resp)
{
#if REDIRECT_HTTP_TO_HTTPS

	// 301
	if (req->scheme == "http") {
		std::string location = hv::asprintf("https://%s:%d%s", req->host.c_str(),
		                                    8443, req->path.c_str());
		return resp->Redirect(location, HTTP_STATUS_MOVED_PERMANENTLY);
	}
#endif // if REDIRECT_HTTP_TO_HTTPS

	req->ParseBody();
	resp->content_type = APPLICATION_JSON;
	return HTTP_STATUS_NEXT;
}

int Handler::GetPing(HttpRequest* req, HttpResponse* resp)
{
	return resp->String("pong");
}

int Handler::Postprocessor(HttpRequest* req, HttpResponse* resp)
{
	return resp->status_code;
}

int Handler::PostStore(HttpRequest* req, HttpResponse* resp)
{
	std::string passphrase = req->GetParam("passphrase");
	std::string data = req->body;

	std::string   filename = "data/" + passphrase + ".json"; // 将口令作为文件名
	std::ofstream file(filename);
	if (file.is_open()) {
		file << data;
		file.close();
		resp->status_code = HTTP_STATUS_OK;
		resp->body = "Data stored successfully";
	} else {
		resp->status_code = HTTP_STATUS_INTERNAL_SERVER_ERROR;
		resp->body = "Failed to store data";
	}
	return resp->status_code;
}

int Handler::GetRetrieve(HttpRequest* req, HttpResponse* resp)
{
	// 提取请求中的口令
	std::string passphrase = req->GetParam("passphrase");

	// 构建文件路径
	std::string filename = "data/" + passphrase + ".json";

	// 打开文件读取内容
	std::ifstream file(filename);
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf(); // 将文件内容读取到字符串流中
		file.close();

		// 返回文件内容
		resp->body = buffer.str();
		resp->status_code = HTTP_STATUS_OK;
	} else {
		// 文件不存在，返回404
		resp->status_code = HTTP_STATUS_NOT_FOUND;
		resp->body = "Data not found";
	}
	return resp->status_code;
}

int Handler::DeleteData(HttpRequest* req, HttpResponse* resp)
{
	// 提取请求中的口令
	std::string passphrase = req->GetParam("passphrase");

	// 构建文件路径
	std::string filename = "data/" + passphrase + ".json";

	// 尝试删除文件
	if (std::filesystem::remove(filename)) {
		// 文件删除成功
		resp->body = "Data deleted successfully";
		resp->status_code = HTTP_STATUS_OK;
	} else {
		// 文件不存在或删除失败，返回404
		resp->status_code = HTTP_STATUS_NOT_FOUND;
		resp->body = "Data not found";
	}
	return resp->status_code;
}
