#include "httpserverrunable.h"
#include "httpclientagent.h"
#include "wrappkiagent.h"
#include "confighandle.h"
#include "parsonjsonagent.h"
#include "utils.h"
#include <httplib.h>
#include <QtCore/QDir>
#include <QtCore/QUuid>
#include <QtCore/QCoreApplication>
#include <algorithm>

struct HttpUtils
{
    static std::string log(const Request &req, const Response &res);
    static std::string dump_headers(const Headers &headers);
    static bool ParseData(const Request &req, AppData &data);
    static bool PackgeJson(std::string &repBody, int code, const std::string &data);

};

HttpServerRunable::HttpServerRunable()
    : m_serverPtr(NULL), m_bExit(false)
{
    InitErrorInfoMap();
}

HttpServerRunable::~HttpServerRunable()
{
    LOG_INFO << "HttpServerRunable::~HttpServerRunable() begin...";
    ShutDown();
    DestroyServer();
    LOG_INFO << "HttpServerRunable::~HttpServerRunable() end...";
}

HttpServerRunable *HttpServerRunable::Instance()
{
    static HttpServerRunable inst;
    return &inst;
}

void HttpServerRunable::ShutDown()
{
    m_bExit = true;
}

void HttpServerRunable::run()
{
    LOG_INFO << "HttpServerRunable::run() begin...";
    while (!m_bExit)
    {
        InitServer();
        GCM::Utils::Sleep(2000);
    }
    LOG_INFO << "HttpServerRunable::run() end...";
}

void HttpServerRunable::InitRouter()
{
    LOG_INFO << "HttpServerRunable::InitRouter() begin...";

    m_serverPtr->Get("/hi", [](const Request &req, Response &res) {
        res.set_content("Hello World!\n", "text/plain");
    });

    ///获取介质信息
    m_serverPtr->Post("/media", [&](const Request &req, Response &res) {
        DevInfoHandle(&req, &res);
    });

    ///证书请求
    m_serverPtr->Post("/cert/request", [&](const Request &req, Response &res) {
        CertRequestHandle(&req, &res);
    });

    ///安装证书
    m_serverPtr->Post("/cert/install", [&](const Request &req, Response &res) {
        CertInstallHandle(&req, &res);
    });

    ///删除证书
    m_serverPtr->Post("/cert/destroy", [&](const Request &req, Response &res) {
        CertDeleteHandle(&req, &res);
    });

    ///解锁介质
    m_serverPtr->Post("/cert/unlock", [&](const Request &req, Response &res) {
        UnLockHandle(&req, &res);
    });

    ///获取票据
    m_serverPtr->Post("/sso/v1/st", [&](const Request &req, Response &res) {
        GetST(&req, &res);
    });

    ///注销会话
    m_serverPtr->Get("/sso/v1/logout", [&](const Request &req, Response &res) {
        Logout(&req, &res);
    });

    LOG_INFO << "HttpServerRunable::InitRouter() end...";
}

void HttpServerRunable::InitErrorInfoMap()
{
    m_errorInfoMap[ERR_PARSON_PARA_ERROR] = "parson parameter failed!";
    m_errorInfoMap[ERR_KEY_NOT_EXIST] = "the longmai key not exist, please insert the longmai key!";
    m_errorInfoMap[ERR_GET_CERT_ERROR] = "get cert from longmai key failed!";
    m_errorInfoMap[ERR_GET_MB_ERROR] = "get MB info from cert failed!";
    m_errorInfoMap[ERR_SIGNATURE_FAILED] = "signature the random data that obtain from sso failed!";
    m_errorInfoMap[ERR_GET_TGT_FAILED] = "get TGT data from sso server failed!";
    m_errorInfoMap[ERR_GET_ST_FAILED] = "get ST data from sso server failed!";
    m_errorInfoMap[ERR_LOGOUT_FAILED] = "logout from sso server failed!";
}

void HttpServerRunable::DevInfoHandle(const Request *req, Response *res)
{
    LOG_INFO << "HttpServerRunable::DevInfoHandle() begin...";
    DeviceInfo info;
    WrapPkiAgent::GetInstance()->MediaInfo(info);

    std::string strResBody = ParsonJsonAgent::BuildMediaInfoBody(req->body, info);
    res->set_header("Access-Control-Allow-Origin", "*");
    res->set_content(strResBody, CONTENT_TYPE);
    LOG_INFO << "HttpServerRunable::DevInfoHandle() end...";
}

void HttpServerRunable::CertRequestHandle(const Request *req, Response *res)
{
    LOG_INFO << "HttpServerRunable::CertRequestHandle() begin...";
    CertRequestInfo info;
    ParsonJsonAgent::ParsonCertResBody(req->body, info);

    std::string strRespJson;

    do
    {
        std::string strUuid = QUuid::createUuid().toString().toLocal8Bit().data();
        bool bRet = WrapPkiAgent::GetInstance()->MakePkcs10(strUuid, info.dn, info.algorithm, info.pubKeyLength, 1, strRespJson);
        if (!bRet)
        {
            res->status = GCM_SERVER_ERROR;
            strRespJson = ParsonJsonAgent::BuildErrorResBody(req->body, ERR_CERT_REQUST_ERROR,
                                                             "cert request failed!", "please check your parameter!");
            LOG_INFO << "MakePkcs10 error!";
            break;
        }

        Pkcs10Info pkcsInfo;
        ParsonJsonAgent::ParsonP10Content(strRespJson, pkcsInfo);

        strRespJson = ParsonJsonAgent::BuildCertResBody(req->body, pkcsInfo);
    } while (0);

    res->set_header("Access-Control-Allow-Origin", "*");
    res->set_content(strRespJson, CONTENT_TYPE);
    LOG_INFO << "HttpServerRunable::CertRequestHandle() end...";
}

void HttpServerRunable::CertInstallHandle(const Request *req, Response *res)
{
    LOG_INFO << "HttpServerRunable::CertInstallHandle() begin...";

    bool bRet = true;
    std::string strRespJson;
    do
    {
        std::string container = WrapPkiAgent::GetInstance()->DefualtContainer();
        if (container.empty())
        {
            bRet = false;
            break;
        }

        CertInfo info;
        ParsonJsonAgent::ParsonReqCertInfo(req->body, info);

        if (!info.certSign.empty())
        {
            bRet = WrapPkiAgent::GetInstance()->ImportX509Cert(container, info.certSign, "1", strRespJson);
            if (!bRet) break;
        }
        if (!info.certEnc.empty())
        {
            bRet = WrapPkiAgent::GetInstance()->ImportX509Cert(container, info.certEnc, "0", strRespJson);
            if (!bRet) break;
        }
    } while (0);

    strRespJson = ParsonJsonAgent::BuildEmptyResBody(req->body);
    if (!bRet)
    {
        res->status = GCM_SERVER_ERROR;
        strRespJson = ParsonJsonAgent::BuildErrorResBody(req->body, ERR_CERT_IMPORT_ERROR,
                                                         "import sign cert failed!", "please check your cert format!");
        LOG_INFO << "ImportX509Cert error!";
    }

    res->set_header("Access-Control-Allow-Origin", "*");
    res->set_content(strRespJson, CONTENT_TYPE);
    LOG_INFO << "HttpServerRunable::CertInstallHandle() begin...";
}

void HttpServerRunable::CertDeleteHandle(const Request *req, Response *res)
{
    LOG_INFO << "HttpServerRunable::CertDeleteHandle() begin...";
    std::vector<std::string> certSnVec;
    ParsonJsonAgent::ParsonCertSnVector(req->body, certSnVec);

    std::string strRespJson;
    bool bRet = true;

    do
    {
        bRet = WrapPkiAgent::GetInstance()->GetAllCert(strRespJson);
        if (!bRet) break;

        std::vector<AllCertInfo> certVec;
        ParsonJsonAgent::ParsonAllCertVector(strRespJson, certVec);
        for (int i = 0; i < certSnVec.size(); ++i)
        {
            auto iter = std::find_if(certVec.begin(), certVec.end(), [&](AllCertInfo info) {
                    return info.SN == certSnVec[i];
        });

            if (iter == certVec.end()) continue;
            WrapPkiAgent::GetInstance()->DelContainer(iter->containerName, strRespJson);
        }

    } while (0);


    strRespJson = ParsonJsonAgent::BuildEmptyResBody(req->body);
    if (!bRet)
    {
        res->status = GCM_SERVER_ERROR;
        strRespJson = ParsonJsonAgent::BuildErrorResBody(req->body, ERR_CERT_DELETE_ERROR,
                                                         "delete cert failed!", "please check your cert serial!");
        LOG_INFO << "DelContainer  error!";
    }

    res->set_header("Access-Control-Allow-Origin", "*");
    res->set_content(strRespJson, CONTENT_TYPE);
    LOG_INFO << "HttpServerRunable::CertDeleteHandle() end...";
}

void HttpServerRunable::UnLockHandle(const Request *req, Response *res)
{
    LOG_INFO << "HttpServerRunable::UnLoclHandle() begin...";
    UnLockPin info;
    ParsonJsonAgent::ParsonUnloclResBody(req->body, info);

    std::string strRespJson;
    bool bRet = WrapPkiAgent::GetInstance()->UnlockPIN(info.adminPIN, info.newUserPIN, strRespJson);

    strRespJson = ParsonJsonAgent::BuildEmptyResBody(req->body);
    if (!bRet)
    {
        res->status = GCM_SERVER_ERROR;
        strRespJson = ParsonJsonAgent::BuildErrorResBody(req->body, ERR_PIN_UNLOCK_ERROR,
                                                         "unlock PIN failed!", "please query your admin PIN!");
    }

    res->set_header("Access-Control-Allow-Origin", "*");
    res->set_content(strRespJson, CONTENT_TYPE);
    LOG_INFO << "HttpServerRunable::UnLoclHandle() end...";
}

void HttpServerRunable::GetST(const Request *req, Response *res)
{
    LOG_INFO << "HttpServerRunable::GetST() begin...";
    AppData data;
    std::string repBody;
    bool bRet = HttpUtils::ParseData(*req, data);
    if (bRet) {
        std::string strST;
        bRet = httpClientAgent::Instance()->GetST(data, strST);
        if (bRet)
            HttpUtils::PackgeJson(repBody, ERR_SUCCESS, strST);
        else
            HttpUtils::PackgeJson(repBody, m_errCode, m_errorInfoMap[m_errCode]);
    }
    else
        HttpUtils::PackgeJson(repBody, ERR_PARSON_PARA_ERROR, "parson parameter failed!");

    res->set_header("Access-Control-Allow-Origin", "*");
    res->set_content(repBody, "application/json;charset=UTF-8");
    LOG_INFO << "HttpServerRunable::GetST() end...";
}

void HttpServerRunable::Logout(const Request *req, Response *res)
{
    bool bRet = httpClientAgent::Instance()->Logout();
    std::string repBody;
    if (bRet)
        HttpUtils::PackgeJson(repBody, ERR_SUCCESS, "");
    else
        HttpUtils::PackgeJson(repBody, ERR_LOGOUT_FAILED, "logout failed!");
    res->set_header("Access-Control-Allow-Origin", "*");
    res->set_content(repBody, "application/json;charset=UTF-8");
}

bool HttpServerRunable::InitServer()
{
    LOG_INFO << "HttpServerRunable::InitServer() begin...";
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    QString appPath = QCoreApplication::applicationDirPath();
    std::string keyFile  = QDir::toNativeSeparators(appPath + SERVER_PRIVATE_KEY_FILE).toLocal8Bit().data();
    std::string certFile = QDir::toNativeSeparators(appPath + SERVER_CERT_FILE).toLocal8Bit().data();
    m_serverPtr = std::make_shared<SSLServer>(certFile.data(), keyFile.data());
#else
    m_serverPtr = std::make_shared<Server>();
#endif

    if (!m_serverPtr->is_valid()) {
        LOG_INFO <<"server has an error...";
        return false;
    }
    /*
    m_serverPtr->Get("/", [=](const Request &req, Response &res) {
        res.set_redirect("/hi");
    });
    m_serverPtr->Get("/stop",
                     [&](const Request &req, Response &res) { m_serverPtr->stop(); });
    */

    InitRouter();

    m_serverPtr->set_error_handler([](const Request &req, Response &res) {
        if (res.status >= GCM_SERVER_ERROR) return;  ///自定义错误号不进行handler处理

        const char *fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
        char buf[BUFSIZ];
        snprintf(buf, sizeof(buf), fmt, res.status);
        res.set_content(buf, "text/html");
    });

    m_serverPtr->set_logger([](const Request &req, const Response &res) {
        printf("%s", HttpUtils::log(req, res).c_str());
        LOG_INFO << "logger: " << HttpUtils::log(req, res).c_str();
    });

    unsigned short port = 0;
    ConfigHandle handle;
    handle.InitConfig();
    handle.GetLocalServerPort(port);
    m_serverPtr->listen("127.0.0.1", port);

    LOG_INFO << "HttpServerRunable::InitServer() end...";
    return 0;
}

void HttpServerRunable::DestroyServer()
{
    LOG_INFO << "HttpServerRunable::DestroyServer() begin...";
    if (m_serverPtr)
    {
        m_serverPtr->stop();
        m_serverPtr.reset();
    }
    LOG_INFO << "HttpServerRunable::DestroyServer() end...";
}

void HttpServerRunable::SetErrorCode(ResErrCode errCode)
{
    LOG_INFO << "HttpServerRunable::SetErrorCode() begin...";
    m_errCode = errCode;
    LOG_INFO << "HttpServerRunable::SetErrorCode() end...";
}

/// httpUtils
std::string HttpUtils::dump_headers(const Headers &headers)
{
    std::string s;
    char buf[BUFSIZ];

    for (auto it = headers.begin(); it != headers.end(); ++it) {
        const auto &x = *it;
        snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
        s += buf;
    }

    return s;
}

std::string HttpUtils::log(const Request &req, const Response &res)
{
    std::string s;
    char buf[BUFSIZ];

    s += "================================\n";

    snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(),
             req.version.c_str(), req.path.c_str());
    s += buf;

    std::string query;
    for (auto it = req.params.begin(); it != req.params.end(); ++it) {
        const auto &x = *it;
        snprintf(buf, sizeof(buf), "%c%s=%s",
                 (it == req.params.begin()) ? '?' : '&', x.first.c_str(),
                 x.second.c_str());
        query += buf;
    }
    snprintf(buf, sizeof(buf), "%s\n", query.c_str());
    s += buf;

    s += dump_headers(req.headers);

    s += "--------------------------------\n";

    snprintf(buf, sizeof(buf), "%d %s\n", res.status, res.version.c_str());
    s += buf;
    s += dump_headers(res.headers);
    s += "\n";

    if (!res.body.empty()) { s += res.body; }

    s += "\n";

    return s;
}

bool HttpUtils::ParseData(const Request &req, AppData &data)
{
    bool bRet = true;
    const auto &headers = req.headers;
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        if ((*it).first == "SecMark")
            data.secMark = (*it).second;
    }

    const auto &params = req.params;
    for (auto it = params.begin(); it != params.end(); ++it) {
        if ((*it).first == "type")
            data.type = static_cast<SignType>(atoi((*it).second.c_str()));

        if ((*it).first == "username")
            data.userName = (*it).second;

        if ((*it).first == "password")
            data.password = (*it).second;

        if ((*it).first == "service")
            data.appSign = (*it).second;
    }

    return !data.appSign.empty() || !data.userName.empty();
}

bool HttpUtils::PackgeJson(std::string &repBody, int code, const std::string &data)
{
    std::string strMsg = (code == 20000 ? "成功" : "失败");
    repBody = ParsonJsonAgent::BuildSSOResBody(code, strMsg, data);

    return !repBody.empty();
}


