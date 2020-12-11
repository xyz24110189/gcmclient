#include "ukeyverifywidget.h"
#include "wrappkiagent.h"
#include "parsonjsonagent.h"
#include "httpclientagent.h"
#include <pkiAgent4c.h>
#include <QtCore/QDebug>
#include <QtCore/QUuid>

bool agentNotifyImp(const kpkiResp * pResp, void *param)
{
    LOG_INFO << "gentNotifyImp.................";
    //#ifdef _WIN32
    WrapPkiAgent::GetInstance()->AgentNotify(pResp);
    //#endif
    return true;
}

Q_DECLARE_METATYPE(kpkiResp)

WrapPkiAgent::WrapPkiAgent(QObject *parent)
    : QObject(parent)
{
    m_verifyPtr = std::make_shared<UkeyVerifyWidget>();
    qRegisterMetaType<const kpkiResp *>("const kpkiResp *");
    InitConnection();
}

WrapPkiAgent::~WrapPkiAgent()
{
    ReleaseAgent();
}

WrapPkiAgent *WrapPkiAgent::GetInstance()
{
    static WrapPkiAgent inst;
    return &inst;
}

void WrapPkiAgent::InitConnection()
{
    connect(this, SIGNAL(DeviceChange(const kpkiResp *, bool)), this, SLOT(OnDeviceChange(const kpkiResp *, bool)), Qt::QueuedConnection);
    connect(m_verifyPtr.get(), SIGNAL(VerifyPin(const QString &, const QString &)),
            this, SLOT(OnVerifyPin(const QString &, const QString &)));
}

bool WrapPkiAgent::InitAgent()
{
    LOG_INFO << "WrapPkiAgent::InitAgent()";
    if (!createAgent(&agentNotifyImp, NULL)) {
        LOG_ERROR << "create Agent failed!";
        return false;
    }
    // 登录
    if (!loginAgent("GCMClient", "B0FDA8376FF94BC0A7965BB83A8E11D5", "gcmclient")) {
        LOG_ERROR << "login pkiservice failed!";
        return false;
    }

    LOG_INFO << "InitAgent success!";
    return true;
}

bool WrapPkiAgent::ReleaseAgent()
{
    LOG_INFO << "WrapPkiAgent::ReleaseAgent() begin...";
    // 登出
    logoutAgent();
    // 释放agent
    releaseAgent();

    LOG_INFO << "WrapPkiAgent::ReleaseAgent() end...";
    return true;
}

void WrapPkiAgent::OnDeviceChange(const kpkiResp *resp, bool isExec)
{
    LOG_INFO << "WrapPkiAgent::OnDeviceChange begin...  type = " << resp->msgType;
    switch (resp->msgType) {
    case 0x0FFF0001: { ///设备插入
        bool bRet = GetDefualtDevice();
        if (!bRet) return;

        m_verifyPtr->SetAppList(m_defaultDevice.appVec);

        if (isExec)	m_verifyPtr->exec();
        else m_verifyPtr->show();

        LOG_INFO << "key insert";
        break;
    }
    case 0x0FFF0002: { ///设备拔出
        bool bRet = ExtractLongmaiDev(resp);
        if (!bRet) return;

        m_verifyPtr->hide();
        m_verifyPtr->SetVerify(false);

        emit AuthChange(false);
        httpClientAgent::Instance()->Logout();
        LOG_INFO << "key remove";
        break;
    }
    case 0x0FFF0003: { ///设备修改
        LOG_INFO << "key changed";
        break;
    }
    case 0x0FFF0004: { ///Session关闭
        LOG_INFO << "session closed";
        break;
    }
    }
    LOG_INFO << "WrapPkiAgent::OnDeviceChange end...";
}

void WrapPkiAgent::OnVerifyPin(const QString &app, const QString &pin)
{
    LOG_INFO << "WrapPkiAgent::OnVerifyPin begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_DEVICE_VERIFYPIN;

    m_defualtApp = app.toLocal8Bit().data();
    std::string json = ParsonJsonAgent::BuildVerifyPINReq(m_defaultDevice.devID.c_str(),
                                                          m_defualtApp.data(),
                                                          "1",
                                                          pin.toLocal8Bit().data());

    req.data->setSize(json.length() + 1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(DEVSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    VerifyPinInfo info;
    ParsonJsonAgent::ParsonVerifyPINRes(resp.data->getDataString(), info);

    bool bResult = (resp.errCode == 0);
    emit AuthChange(bResult);
    m_verifyPtr->SetVerify(bResult);
    m_verifyPtr->ShowInfomationBox(resp.errCode, info);
    LOG_INFO << "WrapPkiAgent::OnVerifyPin end...";
}

void WrapPkiAgent::AgentNotify(const kpkiResp *resp, bool isExec)
{
    emit DeviceChange(resp, isExec);
}

std::string WrapPkiAgent::DefualtContainer()
{
    if (m_defaultDevice.defaultContent.empty())
        GetContainers(m_defaultDevice.devID, m_defualtApp);
    if (m_defaultDevice.defaultContent.empty())
    {
        std::string strResp;
        std::string strUuid = QUuid::createUuid().toString().toLocal8Bit().data();
        bool bRet = CreateContainer(strUuid, strResp);
        if (bRet)
            m_defaultDevice.defaultContent = strUuid;
    }
    return m_defaultDevice.defaultContent;
}

bool WrapPkiAgent::MediaInfo(DeviceInfo &info)
{
    LOG_INFO << "WrapPkiAgent::MediaInfo begin...";
    bool bRet = GetDefualtDevice();
    if (!bRet) return false;

    info = m_defaultDevice;
    LOG_INFO << "info.devID = " << info.devID << "info.devName = " << info.devName.data();
    LOG_INFO << "WrapPkiAgent::MediaInfo end...";
    return bRet;
}

bool WrapPkiAgent::UnlockPIN(const std::string &adminPIN,
                             const std::string &userPIN,
                             std::string &respJson)
{
    LOG_INFO << "WrapPkiAgent::UnlockPIN begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_DEVICE_UNLOCKPIN;

    std::string json = ParsonJsonAgent::BuildUnlockPINReq(m_defaultDevice.devID,
                                                          m_defualtApp.data(),
                                                          adminPIN, userPIN);

    req.data->setSize(json.length() + 1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(DEVSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    respJson = resp.data->getDataString();

    VerifyPinInfo info;
    ParsonJsonAgent::ParsonVerifyPINRes(resp.data->getDataString(), info);
    m_verifyPtr->ShowInfomationBox(resp.errCode, info);

    LOG_INFO << "WrapPkiAgent::UnlockPIN end...";
    return resp.errCode == 0;
}

bool WrapPkiAgent::CreateContainer(const std::string &containerName,
                                   std::string &respJson)
{
    LOG_INFO << "WrapPkiAgent::CreateContainer begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_DEVICE_CREATECONTAINER;

    bool bRet = VerifyPINJudge();
    if (!bRet) return false;

    std::string json = ParsonJsonAgent::BuildCreateContainerReq(m_defaultDevice.devID,
                                                                m_defualtApp.data(),
                                                                containerName);

    req.data->setSize(json.length() + 1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(DEVSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    respJson = resp.data->getDataString();

    LOG_INFO << "WrapPkiAgent::CreateContainer end...";
    return resp.errCode == 0;
}

bool WrapPkiAgent::GetContainers(const std::string &devId, const std::string &appName)
{
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_DEVICE_GETCONTAINERS;

    std::string json = ParsonJsonAgent::BuildGetContainersReq(devId, appName);

    req.data->setSize(json.length() + 1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(DEVSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    std::vector<std::string> conVec;
    ParsonJsonAgent::ParsonContainerVector(resp.data->getDataString(), conVec);

    if (conVec.size() > 0) m_defaultDevice.defaultContent = conVec[0];
    return resp.errCode == 0;

}

bool WrapPkiAgent::GenKeypair(const std::string &conName,
                              const std::string &keyType,
                              const std::string &keyLen,
                              const uint32_t &purpose,
                              std::string &respJson)
{
    LOG_INFO << "WrapPkiAgent::GenKeypair begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_ENROLL_KEYPAIR;

    bool bRet = VerifyPINJudge();
    if (!bRet) return false;

    std::string json = ParsonJsonAgent::BuildGenb64KeypairReq(m_defaultDevice.devID,
                                                              m_defualtApp.data(),
                                                              conName, keyType,
                                                              keyLen, purpose);

    req.data->setSize(json.length() + 1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(ENROLLSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    respJson = resp.data->getDataString();

    LOG_INFO << "WrapPkiAgent::GenKeypair end...";
    return resp.errCode == 0;
}

bool WrapPkiAgent::MakePkcs10(const std::string &conName,
                              const std::string &dn,
                              const std::string &algrithm,
                              const std::string &keyLength,
                              const int &extensionType,
                              std::string &respJson)
{
    LOG_INFO << "WrapPkiAgent::MakePkcs10 begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_ENROLL_MKP10;

    bool bRet = VerifyPINJudge();
    if (!bRet) return false;

    bRet = CreateContainer(conName, respJson);
    if (!bRet)
    {
        LOG_INFO << "CreateContainer failed1";
        return false;
    }
    m_defaultDevice.defaultContent = conName;

    std::string keyType = (algrithm.compare("RSA") == 0) ? "1" : "0";
    bRet = GenKeypair(conName, keyType, keyLength, 1, respJson);
    if (!bRet)
    {
        LOG_INFO << "GenKeypair failed1";
        return false;
    }


    std::string json = ParsonJsonAgent::BuildMakePkcs10Req(m_defaultDevice.devID,
                                                           m_defualtApp.data(),
                                                           conName, dn, extensionType);

    req.data->setSize(json.length() + 1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(ENROLLSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    respJson = resp.data->getDataString();

    LOG_INFO << "WrapPkiAgent::MakePkcs10 end...";
    return resp.errCode == 0;

}

bool WrapPkiAgent::ImportX509Cert(const std::string &conName,
                                  const std::string &b64cert,
                                  const std::string &purpose,
                                  std::string &respJson)
{
    LOG_INFO << "WrapPkiAgent::ImportX509Cert begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_ENROLL_IMPORTX509;

    bool bRet = VerifyPINJudge();
    if (!bRet) return false;

    std::string json = ParsonJsonAgent::BuildInstallCertReq(m_defaultDevice.devID,
                                                            m_defualtApp.data(),
                                                            conName, b64cert, purpose);

    req.data->setSize(json.length() + 1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(ENROLLSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    respJson = resp.data->getDataString();

    LOG_INFO << "WrapPkiAgent::ImportX509Cert end...";
    return resp.errCode == 0;
}

bool WrapPkiAgent::DelContainer(const std::string &containerName,
                                std::string &respJson)
{
    LOG_INFO << "WrapPkiAgent::DelContainer begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_DEVICE_DELCONTAINER;

    bool bRet = VerifyPINJudge();
    if (!bRet) return false;

    std::string json = ParsonJsonAgent::BuildDelContainerReq(m_defaultDevice.devID,
                                                             m_defualtApp.data(),
                                                             containerName);

    req.data->setSize(json.length() + 1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(DEVSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    respJson = resp.data->getDataString();

    LOG_INFO << "WrapPkiAgent::DelContainer end...";
    return resp.errCode == 0;
}

bool WrapPkiAgent::GetAllCert(std::string &respJson)
{
    LOG_INFO << "WrapPkiAgent::GetAllCert begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_DEVICE_GETALLCERT;

    bool bRet = VerifyPINJudge();
    if (!bRet) return false;

    reqSync(DEVSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    respJson = resp.data->getDataString();

    LOG_INFO << "WrapPkiAgent::GetAllCert end...";
    return resp.errCode == 0;
}

bool WrapPkiAgent::GetCert(const std::string &certType, std::string &certData)
{
    LOG_INFO << "WrapPkiAgent::GetCert begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid	= 1;
    req.version = 0x01;
    req.extend	= 0x00;
    req.msgType = MSG_ENROLL_GETCERT;

    bool bRet = VerifyPINJudge();
    if (!bRet) return false;

    std::string json = ParsonJsonAgent::BuildGetb64certReq(m_defaultDevice.devID,
                                                           m_defualtApp,
                                                           DefualtContainer(),
                                                           certType);

    req.data->setSize(json.length()+1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(ENROLLSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    ParsonJsonAgent::ParseCertRes(resp.data->getDataString(), certData);
    LOG_INFO << "WrapPkiAgent::GetCert end...";
    return resp.errCode == 0;

}

bool WrapPkiAgent::ParseCertMb(const std::string &cert, std::string &strMb)
{
    LOG_INFO << "WrapPkiAgent::ParseCert begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid	= 1;
    req.version = 0x01;
    req.extend	= 0x00;
    req.msgType = MSG_SIGNX_PARSECERT;

    bool bRet = VerifyPINJudge();
    if (!bRet) return false;

    std::string json = ParsonJsonAgent::BuildCertParseReq(cert);

    req.data->setSize(json.length()+1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(SIGNXSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    ParsonJsonAgent::ParseMbFromCert(resp.data->getDataString().c_str(), strMb);
    LOG_INFO << "WrapPkiAgent::ParseCert end...";
    return resp.errCode == 0;
}

bool WrapPkiAgent::SignData(const std::string &srcData,
                            const unsigned int &type,
                            std::string &signData)
{
    LOG_INFO << "WrapPkiAgent::SignData begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid	= 1;
    req.version = 0x01;
    req.extend	= 0x00;
    req.msgType = MSG_SIGNX_SIGNDATA;

    std::string json = ParsonJsonAgent::BuildSignDataReq(m_defaultDevice.devID,
                                                         m_defualtApp,
                                                         DefualtContainer(),
                                                         srcData,
                                                         type);

    req.data->setSize(json.length()+1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(SIGNXSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    /// 获取签名结果
    ParsonJsonAgent::ParseSignDatResp(resp.data->getDataString(), signData);
    LOG_INFO << "WrapPkiAgent::SignData end...";
    return resp.errCode == 0;
}


bool WrapPkiAgent::P7Sign(const std::string &srcData,
                          const unsigned int &mdType,
                          const unsigned int &attachData,
                          std::string &signData)
{
    LOG_INFO << "WrapPkiAgent::P7Sign begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid	= 1;
    req.version = 0x01;
    req.extend	= 0x00;
    req.msgType = MSG_SIGNX_SIGNP7;

    bool bRet = VerifyPINJudge();
    if (!bRet) return false;

    std::string json = ParsonJsonAgent::BuildPkcs7SignReq(m_defaultDevice.devID,
                                                          m_defualtApp,
                                                          DefualtContainer(),
                                                          srcData,
                                                          mdType,
                                                          attachData);

    req.data->setSize(json.length()+1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(SIGNXSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    /// 获取签名结果
    ParsonJsonAgent::ParsePkcs7SignRes(resp.data->getDataString(), signData);

    LOG_INFO << "WrapPkiAgent::P7Sign end...";
    return resp.errCode == 0;

}

bool WrapPkiAgent::GetDefualtDevice()
{
    LOG_INFO << "WrapPkiAgent::GetDefualtDevice begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_DEVICE_GETDEVICES;

    reqSync(DEVSERVICE, &req, &resp);

    LOG_INFO << "res.data= " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    ParsonJsonAgent::deviceVecType devVec;
    ParsonJsonAgent::ParsonDeviceList(resp.data->getDataString(), devVec);
    //提取一个devID
    ParsonJsonAgent::deviceVecType::iterator iter = devVec.begin();
    if (devVec.size() == 0)
    {
        LOG_INFO << "without any SyncDevice, process exit";
        return false;
    }

    DeviceDetialInfo info;
    while (iter != devVec.end())
    {
        if (IsLongmaiDev(iter->devID, info))
        {
            LOG_INFO << "IsLongmaiDev find manufacturer = " << info.manufacturer.c_str()
                     << "issuer" << info.issuer.c_str();
            m_defaultDevice = *iter;
            m_defaultDevice.detailInfo = info;
            break;
        }
        ++iter;
    }
    LOG_INFO << "devid= " << m_defaultDevice.devID.c_str();
    //return resp.errCode == 0;
    LOG_INFO << "WrapPkiAgent::GetDefualtDevice end...";

    if (iter == devVec.end()) return false;

    return GetAppList(m_defaultDevice.devID);
}

bool WrapPkiAgent::IsLongmaiDev(const std::string &devID/*in*/, DeviceDetialInfo &info/*out*/)
{
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_DEVICE_GETDEVINFO;

    std::string json = ParsonJsonAgent::BuildGetDevInfoReq(devID);

    req.data->setSize(json.length() + 1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(DEVSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;
    if (resp.errCode == 0)
    {
        ParsonJsonAgent::ParsonDeviceDetailInfo(resp.data->getDataString(), info);
        if (info.manufacturer.find("Longmai") != std::string::npos ||
                info.issuer.find("Longmai") != std::string::npos)
            return true;
    }

    return false;
}

bool WrapPkiAgent::GetAppList(const std::string &devID)
{
    LOG_INFO << "WrapPkiAgent::GetAppList begin...";
    kpkiReq req;
    kpkiResp resp;
    req.reqid = 1;
    req.version = 0x01;
    req.extend = 0x00;
    req.msgType = MSG_DEVICE_GETAPPLIST;

    std::string json = ParsonJsonAgent::BuildGetAppListReq(devID);

    req.data->setSize(json.length() + 1);
    strncpy((char *)req.data->getData(), json.c_str(), json.length());

    reqSync(DEVSERVICE, &req, &resp);
    LOG_INFO << "res.data = " << resp.data->getDataString().c_str();
    LOG_INFO << "res.errCode = " << resp.errCode;

    ParsonJsonAgent::appVecType appVec;
    ParsonJsonAgent::ParsonAppList(resp.data->getDataString(), appVec);

    if (appVec.size() == 0)
    {
        LOG_INFO << "the device appList is empty!";
        return false;
    }

    m_defaultDevice.appVec = appVec;

    LOG_INFO << "WrapPkiAgent::GetAppList end...";
    return resp.errCode == 0;
}

bool WrapPkiAgent::VerifyPINJudge()
{
    if (m_defaultDevice.devID.empty() || !m_verifyPtr->Verify())
    {
        kpkiResp resp;
        resp.msgType = UkeyOperation::INSERT;
        AgentNotify(&resp, true);
    }

    return m_verifyPtr->Verify();
}

bool WrapPkiAgent::ExtractLongmaiDev(const kpkiResp *resp)
{
    DeviceBriefInfo info;
    ParsonJsonAgent::ParsonDeviceBriefInfo(resp->jsonBody, info);
    if (!info.devID.compare(m_defaultDevice.devID) &&
            !info.devNumber.compare(m_defaultDevice.devNumber))
        return true;
    return false;
}



