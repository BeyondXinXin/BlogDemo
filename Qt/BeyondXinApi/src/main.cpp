#include "HttpServer.h"
#include "handler.h"
#include "hasync.h" // import hv::async
#include "hmain.h"
#include "hssl.h"
#include "hv.h"
#include "iniparser.h"
#include "router.h"

hv::HttpServer g_http_server;
hv::HttpService g_http_service;

static int parse_confile(const char * confile);

int parse_confile(const char * confile)
{
    IniParser ini;
    int ret = ini.LoadFromFile(confile);
    if (ret != 0) {
        printf("Load confile [%s] failed: %d\n", confile, ret);
        exit(-40);
    }

    // logfile
    std::string str = ini.GetValue("logfile");
    if (!str.empty()) {
        strncpy(g_main_ctx.logfile, str.c_str(), sizeof(g_main_ctx.logfile));
    }
    hlog_set_file(g_main_ctx.logfile);
    // loglevel
    str = ini.GetValue("loglevel");
    if (!str.empty()) {
        hlog_set_level_by_str(str.c_str());
    }
    // log_filesize
    str = ini.GetValue("log_filesize");
    if (!str.empty()) {
        hlog_set_max_filesize_by_str(str.c_str());
    }
    // log_remain_days
    str = ini.GetValue("log_remain_days");
    if (!str.empty()) {
        hlog_set_remain_days(atoi(str.c_str()));
    }
    // log_fsync
    str = ini.GetValue("log_fsync");
    if (!str.empty()) {
        logger_enable_fsync(hlog, hv_getboolean(str.c_str()));
    }
    hlogi("%s version: %s", g_main_ctx.program_name, hv_compile_version());
    hlog_fsync();

    // worker_processes
    int worker_processes = 0;

    str = ini.GetValue("worker_processes");
    if (str.size() != 0) {
        if (strcmp(str.c_str(), "auto") == 0) {
            worker_processes = get_ncpu();
            hlogd("worker_processes=ncpu=%d", worker_processes);
        } else {
            worker_processes = atoi(str.c_str());
        }
    }

    // g_http_server.Static("/", "./html");
    g_http_server.worker_processes =
      LIMIT(0, worker_processes, MAXNUM_WORKER_PROCESSES);
    // worker_threads
    int worker_threads = 0;
    str = ini.GetValue("worker_threads");
    if (str.size() != 0) {
        if (strcmp(str.c_str(), "auto") == 0) {
            worker_threads = get_ncpu();
            hlogd("worker_threads=ncpu=%d", worker_threads);
        } else {
            worker_threads = atoi(str.c_str());
        }
    }
    g_http_server.worker_threads = LIMIT(0, worker_threads, 64);

    // worker_connections
    str = ini.GetValue("worker_connections");
    if (str.size() != 0) {
        g_http_server.worker_connections = atoi(str.c_str());
    }

    // port
    g_http_server.port = ini.Get<int>("http_port");
    g_http_server.https_port = 0;

    // document_root
    str = ini.GetValue("document_root");
    if (str.size() != 0) {
        g_http_service.document_root = str;
    }
    // home_page
    str = ini.GetValue("home_page");
    if (str.size() != 0) {
        g_http_service.home_page = str;
    }

    // keepalive_timeout
    str = ini.GetValue("keepalive_timeout");
    g_http_service.keepalive_timeout = atoi(str.c_str());
    // limit_rate
    str = ini.GetValue("limit_rate");
    g_http_service.limit_rate = atoi(str.c_str());
    // access_log
    str = ini.GetValue("access_log");
    g_http_service.enable_access_log = hv_getboolean(str.c_str());
    // cors
    g_http_service.AllowCORS();

    // ssl
    if (g_http_server.https_port > 0) {
        std::string crt_file = ini.GetValue("ssl_certificate");
        std::string key_file = ini.GetValue("ssl_privatekey");
        std::string ca_file = ini.GetValue("ssl_ca_certificate");
        hlogi("SSL backend is %s", hssl_backend());
        hssl_ctx_opt_t param;
        memset(&param, 0, sizeof(param));
        param.crt_file = crt_file.c_str();
        param.key_file = key_file.c_str();
        param.ca_file = ca_file.c_str();
        param.endpoint = HSSL_SERVER;
        if (g_http_server.newSslCtx(&param) != 0) {
#ifdef OS_WIN
            if (strcmp(hssl_backend(), "schannel") == 0) {
                hlogw("schannel needs pkcs12 formatted certificate file.");
                g_http_server.https_port = 0;
            }
#else
            hloge("SSL certificate verify failed!");
            exit(0);
#endif
        } else {
            hlogi("SSL certificate verify ok!");
        }
    }
    // proxy
    auto proxy_keys = ini.GetKeys("proxy");
    for (const auto & proxy_key : proxy_keys) {
        str = ini.GetValue(proxy_key, "proxy");
        if (str.empty())
            continue;
        if (proxy_key[0] == '/') {
            // reverse proxy
            const std::string & path = proxy_key;
            std::string proxy_url = hv::ltrim(str, "> ");
            hlogi("reverse_proxy %s => %s", path.c_str(), proxy_url.c_str());
            g_http_service.Proxy(path.c_str(), proxy_url.c_str());
        } else if (strcmp(proxy_key.c_str(), "proxy_connect_timeout") == 0) {
            g_http_service.proxy_connect_timeout = atoi(str.c_str());
        } else if (strcmp(proxy_key.c_str(), "proxy_read_timeout") == 0) {
            g_http_service.proxy_read_timeout = atoi(str.c_str());
        } else if (strcmp(proxy_key.c_str(), "proxy_write_timeout") == 0) {
            g_http_service.proxy_write_timeout = atoi(str.c_str());
        } else if (strcmp(proxy_key.c_str(), "forward_proxy") == 0) {
            hlogi("forward_proxy = %s", str.c_str());
            if (hv_getboolean(str.c_str())) {
                g_http_service.EnableForwardProxy();
            }
        } else if (strcmp(proxy_key.c_str(), "trust_proxies") == 0) {
            auto trust_proxies = hv::split(str, ';');
            for (auto trust_proxy : trust_proxies) {
                trust_proxy = hv::trim(trust_proxy);
                if (trust_proxy.empty())
                    continue;
                hlogi("trust_proxy %s", trust_proxy.c_str());
                g_http_service.AddTrustProxy(trust_proxy.c_str());
            }
        } else if (strcmp(proxy_key.c_str(), "no_proxies") == 0) {
            auto no_proxies = hv::split(str, ';');
            for (auto no_proxy : no_proxies) {
                no_proxy = hv::trim(no_proxy);
                if (no_proxy.empty())
                    continue;
                hlogi("no_proxy %s", no_proxy.c_str());
                g_http_service.AddNoProxy(no_proxy.c_str());
            }
        }
    }

    // GithubAuthorize
    str = ini.GetValue("client_id");
    Handler::m_ClientId = str;
    str = ini.GetValue("client_secret");
    Handler::m_ClientSecret = str;

    hlogi("parse_confile('%s') OK", confile);
    return 0;
}

static void on_reload(void * userdata)
{
    hlogi("reload confile [%s]", g_main_ctx.confile);
    parse_confile(g_main_ctx.confile);
}

int main(int argc, char ** argv)
{
    // g_main_ctx
    main_ctx_init(argc, argv);

    parse_confile("./beyondxin_api.conf");

    // signal
    signal_init(on_reload);

    const char * signal = get_arg("s");
    if (signal) {
        signal_handle(signal);
    }

    create_pidfile();

    Router::Register(g_http_service);
    g_http_server.registerHttpService(&g_http_service);
    g_http_server.run();

    return 0;
}
