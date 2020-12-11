#include "httpclientagent.h"
#include "parsonjsonagent.h"
#include "uniqueAuthDatabase.h"
#include "httpserverrunable.h"
#include "wrappkiagent.h"
#include "confighandle.h"
#include "utils.h"
#include <httplib.h>
#include <QtCore/QUuid>

using namespace httplib;

struct HttpAdapter {
    static bool InitHttpPostHeaders(httplib::Headers &headers);
    static bool InitHttpDeleHeaders(httplib::Headers &headers);
};

bool HttpAdapter::InitHttpPostHeaders(httplib::Headers &headers)
{
    headers.emplace("Accept", "application/json");
    //headers.emplace("SecMark", "03f524439860ef7839b6e15e14182706");
    headers.emplace("Content-Type", "application/x-www-form-urlencoded;charset=UTF-8");
    return true;
}

bool HttpAdapter::InitHttpDeleHeaders(httplib::Headers &headers)
{
    headers.emplace("Accept", "application/json");
    return true;
}

httpClientAgent::httpClientAgent()
{
    InitDB();
    InitClient();
}

httpClientAgent::~httpClientAgent()
{
    DestroyClient();
}

bool httpClientAgent::ReInitClient()
{
    DestroyClient();
    return InitClient();
}

httpClientAgent *httpClientAgent::Instance()
{
    static httpClientAgent inst;
    return &inst;
}

bool httpClientAgent::InitDB()
{
    //初始化数据库
    m_uniqueDb = std::make_shared<UniqueAuthDatabase>();
    return m_uniqueDb->InitDb();
}

bool httpClientAgent::InitClient()
{
    std::string strAddr;
    unsigned short port = 0;

    ConfigHandle handle;
    handle.InitConfig();
    handle.GetRemoteServerConf(strAddr, port);

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    m_clientPtr = std::make_shared<SSLClient>(strAddr, port);
    // httplib::SSLClient cli("google.com");
    // httplib::SSLClient cli("www.youtube.com");
    SSLClient *sslCli = dynamic_cast<SSLClient*>(m_clientPtr.get());
    sslCli->set_ca_cert_path(CA_CERT_FILE);
    sslCli->enable_server_certificate_verification(true);
#else
    m_clientPtr = std::make_shared<Client>(strAddr, port);
#endif

    /*auto res = cli.Get("/hi");
    if (res) {
        cout << res->status << endl;
        cout << res->get_header_value("Content-Type") << endl;
        cout << res->body << endl;
    }
    else {
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        auto result = cli.get_openssl_verify_result();
        if (result) {
            cout << "verify error: " << X509_verify_cert_error_string(result) << endl;
        }
#endif
    }*/

    return m_clientPtr->is_valid();
}

void httpClientAgent::DestroyClient()
{
    if(m_clientPtr)  m_clientPtr.reset();
}


bool httpClientAgent::GetST(const AppData &appData, std::string &strST)
{
    std::string strTGT;
    bool bRet = VerifyLocalTGT(appData, strTGT);
    if (!bRet) return false;

    httplib::Headers headers;
    HttpAdapter::InitHttpPostHeaders(headers);
    httplib::Params params;
    params.emplace("service", appData.appSign);
    params.emplace("tgtId", strTGT);
    std::string url("/sso/v1/st");
    //url += strTGT;
    auto res = m_clientPtr->Post(url.c_str(), headers, params);
    if (res && res->status == 200)
    {
        std::string body = res->body;
        bRet = ParsonJsonAgent::ParsonSSOResBody(body, strST);
        if (!bRet)
        {
            HttpServerRunable::Instance()->SetErrorCode(ERR_GET_ST_FAILED);
            return false;
        }
        return true;
    }
    else
        return false;
}

bool httpClientAgent::Logout()
{
    httplib::Headers headers;
    HttpAdapter::InitHttpPostHeaders(headers);

    std::string strTGT;
    bool bRet = m_uniqueDb->GetLocalTGT(strTGT);
    if (!bRet) return true;

    httplib::Params params;
    params.emplace("tgtId", strTGT);
    std::string url("/sso/v1/delTgt");
    //url += strTGT;
    auto res = m_clientPtr->Post(url.c_str(), headers, params);
    if (res && res->status == 200)
    {
        std::string data;
        std::string body = res->body;
        bRet = ParsonJsonAgent::ParsonSSOResBody(body, data);
        if (!bRet) return false;

        m_uniqueDb->DeleteTGT();
        return true;
    }
    else
        return false;
}

bool httpClientAgent::GetTGT(const AppData &appData, std::string &strTGT)
{
    httplib::Headers headers;
    HttpAdapter::InitHttpPostHeaders(headers);
    std::string macAddr = GCM::Utils::getMacAddress();
    std::string machineID = GCM::Utils::getMachineID();
    headers.emplace("mac", macAddr);
    headers.emplace("terminalId", machineID);

    httplib::Params params;
    char type[8] = {0};
    sprintf(type, "%d", appData.type);
    params.emplace("type", type);

    if (appData.type == SignType::PIN_TYPE)
    {
        params.emplace("username", appData.userName);
        params.emplace("password", appData.password);
    }
    else if (appData.type == SignType::UKEY_TYPE)
    {
        std::string certData;
        bool bRet = WrapPkiAgent::GetInstance()->GetCert("1", certData);
        if (!bRet)
        {
            HttpServerRunable::Instance()->SetErrorCode(ERR_GET_CERT_ERROR);
            bRet = WrapPkiAgent::GetInstance()->VerifyPINJudge();
            if (!bRet)
                HttpServerRunable::Instance()->SetErrorCode(ERR_KEY_NOT_EXIST);

            return false;
        }

        std::string strMb;
        bRet = WrapPkiAgent::GetInstance()->ParseCertMb(certData, strMb);
        if (!bRet || strMb.empty())
        {
            HttpServerRunable::Instance()->SetErrorCode(ERR_GET_MB_ERROR);
            return false;
        }

        DeviceInfo info;
        WrapPkiAgent::GetInstance()->MediaInfo(info);
        headers.emplace("secMark", strMb);
        headers.emplace("keySn", info.devNumber);

        std::string p7Sign;
        bRet = GetP7Sign(p7Sign);
        if (!bRet)
        {
            HttpServerRunable::Instance()->SetErrorCode(ERR_SIGNATURE_FAILED);
            return false;
        }

        params.emplace("cert", certData);
        params.emplace("signResult", p7Sign);
        params.emplace("originalData", m_strRandom);
    }

    LOG_INFO << "get local db TGT failed, obtain from server.";
    auto res = m_clientPtr->Post("/sso/v1/tgt", headers, params);
    if (res && res->status == 200)
    {
        std::string body = res->body;
        bool bRet = ParsonJsonAgent::ParsonSSOResBody(body, strTGT);
        if (!bRet)
        {
            HttpServerRunable::Instance()->SetErrorCode(ERR_GET_TGT_FAILED);
            return false;
        }
        return true;
    }
    else
        return false;
}

bool httpClientAgent::VerifyLocalTGT(const AppData &appData, std::string &strTGT)
{
    bool bRet = m_uniqueDb->QueryLocalTGT(appData, strTGT);
    if (!bRet)
    {
        bRet = GetTGT(appData, strTGT);
        if (!bRet) return false;
        m_uniqueDb->InsertTGT(appData, strTGT);
    }
    else
        LOG_INFO << "get local db TGT success.";
    return true;
}

bool httpClientAgent::GetP7Sign(std::string &p7Sign)
{
    auto res = m_clientPtr->Get("/sso/v1/random");
    if (res && res->status == 200)
    {
        std::string body = res->body;
        bool bRet = ParsonJsonAgent::ParsonRandomResBody(body, m_strRandom);
        if (!bRet) return false;
        return WrapPkiAgent::GetInstance()->SignData(m_strRandom, SignCustom::SM2_RSA, p7Sign);
    }
    else
        return false;
    //m_strRandom = QUuid::createUuid().toByteArray().toBase64().constData();
    //return WrapPkiAgent::GetInstance()->P7Sign(m_strRandom, SignP7Algorithm::SHA1, 0, p7Sign); ///"1"-MD5 "2"-SHA1 "3"-SM3 "4"-SHA256
}






