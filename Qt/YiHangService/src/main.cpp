#include "HttpServer.h"
#include "handler.h"
#include "hasync.h" // import hv::async
#include "hmain.h"
#include "hssl.h"
#include "hv.h"
#include "iniparser.h"
#include "router.h"

hv::HttpServer  gHttpServer;
hv::HttpService gHttpService;

int InitServer()
{
	hlog_set_level_by_str("INFO");
	hlog_set_remain_days(3);
	logger_enable_fsync(hlog, hv_getboolean(std::string("64M").c_str()));
	hlogi("%s version: %s", g_main_ctx.program_name, hv_compile_version());
	hlog_fsync();
	hlogd("worker_processes=ncpu=%d", get_ncpu());
	gHttpServer.worker_processes = LIMIT(0, get_ncpu(), MAXNUM_WORKER_PROCESSES);
	gHttpServer.worker_threads = LIMIT(0, 1, 64);
	gHttpServer.worker_connections = 1024;
	gHttpServer.port = 5677;
	gHttpServer.https_port = 0;
	gHttpService.document_root = "html";
	gHttpService.home_page = "index.html";
	gHttpService.keepalive_timeout = 75000;
	gHttpService.limit_rate = 500;
	gHttpService.enable_access_log = false;
	gHttpService.AllowCORS();

	// ssl
	if (gHttpServer.https_port > 0) {
		std::string crt_file = "./cert/server.crt";
		std::string key_file = "./cert/server.key";
		std::string ca_file = "./cert/cacert.pem";
		hlogi("SSL backend is %s", hssl_backend());
		hssl_ctx_opt_t param;
		memset(&param, 0, sizeof(param));
		param.crt_file = crt_file.c_str();
		param.key_file = key_file.c_str();
		param.ca_file = ca_file.c_str();
		param.endpoint = HSSL_SERVER;
		if (gHttpServer.newSslCtx(&param) != 0) {
#ifdef OS_WIN
			if (strcmp(hssl_backend(), "schannel") == 0) {
				hlogw("schannel needs pkcs12 formatted certificate file.");
				gHttpServer.https_port = 0;
			}
#else // ifdef OS_WIN
			hloge("SSL certificate verify failed!");
			exit(0);
#endif // ifdef OS_WIN
		} else {
			hlogi("SSL certificate verify ok!");
		}
	}

	hlogi("InitServer OK");
	return 0;
}

int main(int argc, char** argv)
{
	main_ctx_init(argc, argv);
	InitServer();

	create_pidfile();

	Router::Register(gHttpService);
	gHttpServer.registerHttpService(&gHttpService);
	gHttpServer.run();

	return 0;
}
