#ifndef WRAPPKIAGENT_H
#define WRAPPKIAGENT_H
#include <QObject>
#include <memory>
#include <string>
#include <map>
#include "commontype.h"

class UkeyVerifyWidget;
struct kpkiResp;
class WrapPkiAgent : public QObject
{
    Q_OBJECT
public:
    static WrapPkiAgent *GetInstance();

    bool InitAgent();

    void AgentNotify(const kpkiResp *resp, bool isExec = false);

	std::string DefualtContainer();

    /**
    * @ brief 获取序列号
    * @ param [out] strSerial 序列号字符串
    * @ return true/false;  true:获取成功, false:获取失败
    */
    bool MediaInfo(DeviceInfo &info);

    /**
    * @ brief 解锁用户PIN码
    * @ param [in] adminPIN   高级管理员PIN码
    * @ param [in] userPIN    用户PIN码
    * @ param [out] respJson  返回数据
    * @ return true/false; true:解锁成功, false:解锁失败
    */
    bool UnlockPIN(const std::string &adminPIN,
                   const std::string &userPIN,
                   std::string &respJson);

    /**
    * @ brief 创建容器
    * @ param [in]  containerName  容器名
    * @ param [out] respJson       返回数据
    * @ return true/false; true:成功, false:失败
    */
    bool CreateContainer(const std::string &containerName,
                         std::string &respJson);

	/**
	* @ brief 获取容器列表
	* @ param  [in] devId    设备ID
	* @ param  [in] appName  应用名
	* @ return true/false; true:成功, false:失败
	*/
	bool GetContainers(const std::string &devId, const std::string &appName);

    /**
     * @ brief 生成密钥对
     * @ param [in] conName   容器名
     * @ param [in] keyType   0:SM2,1:RSA
     * @ param [in] keyLen    keyType为rsa时有效
     * @ param [in] purpose   keyType为SM2时候有效（目前key不支持2）,1. SGD_SM2_1,2. SGD_SM2_3
     * @ param [out] respJson 返回数据
     * @ return true/false; true:成功, false:失败
    */
    bool GenKeypair(const std::string &conName,
                    const std::string &keyType,
                    const std::string &keyLen,
                    const uint32_t &purpose,
                    std::string &respJson);

    /**
    * @ brief 生成p10请求
    * @ param [in] conName       容器名称
    * @ param [in] dn            DN 项
	* @ param [in] algrithm      证书算法
	* @ param [in] keyLength     证书密钥长度
    * @ param [in] extensionType 临时密钥扩展项，1.不携带，2.携带临时密钥，其他值.协同模式
    * @ param [out] respJson     返回数据
    * @ return true/false; true:成功, false:失败
    */
    bool MakePkcs10(const std::string &conName,
                    const std::string &dn,
					const std::string &algrithm,
					const std::string &keyLength,
                    const int &extensionType,
                    std::string &respJson);

    /**
     * @ brief 导入证书
     * @ param [in] conName   容器名
     * @ param [in] b64cert   证书内容
     * @ param [in] purpose   1表示签名, 0表示加密
     * @ param [out] respJson     返回数据
     * @ return true/false; true:成功, false:失败
     */
    bool ImportX509Cert(const std::string &conName,
                        const std::string &b64cert,
                        const std::string &purpose,
                        std::string &respJson);

    /**
    * @ brief 删除容器
    * @ param [in] containerName  容器名
    * @ param [out] respJson      返回数据
    * @ return true/false; true:成功, false:失败
    */
    bool DelContainer(const std::string &containerName,
                      std::string &respJson);

	/**
	* @ brief 获取证书列表
	* @ param [out] respJson  返回数据
	* @ return true/false; true:成功, false:失败
	*/
	bool GetAllCert(std::string &respJson);

    /**
    * @ brief 导出证书
    * @ param [in] certType 1表示签名证书, 0表示加密证书
    * @ param [out] certData 证书内容
    * @ return true/false; true:成功, false:失败
    */
    bool GetCert(const std::string &certType, std::string &certData);

    /**
    * @ brief 解析证书密标信息
    * @ param [in] cert 证书内容(base64编码)
    * @ param [out] strMb 密标信息
    * @ return true/false; true:成功, false:失败
    */
    bool ParseCertMb(const std::string &cert, std::string &strMb);

    /**
    * @ brief  普通签名
    * @ param [in]  srcData   待签名数据(base64编码)
    * @ param [in]  type      签名类型,1表示PM-BD签名,2表示SM2/RSA签名,3 SSL建链定制签名
    * @ param [out] signData  签名结果
    * @ return true/false; true:成功, false:失败
    */
    bool SignData(const std::string &srcData,
                  const unsigned int &type,
                  std::string &signData);

    /**
    * @ brief p7签名
    * @ param [in] srcData    待签名数据(base64编码)
    * @ param [in] mdType     指定的摘要类型,1-MD5 2-SHA1 3-SM3 4-SHA256
    * @ param [in] attachData 0表示PKCS7_DETACHED
    * @ param [out] signData  签名结果
    * @ return true/false; true:成功, false:失败
    */
    bool P7Sign(const std::string &srcData,
                const unsigned int &mdType,
                const unsigned int &attachData,
                std::string &signData);

public slots:
    void OnDeviceChange(const kpkiResp *resp, bool isExec);
    void OnVerifyPin(const QString &app, const QString &pin);

signals:
    void DeviceChange(const kpkiResp *resp, bool isExec);
	void AuthChange(bool auth);

public:
	bool ReleaseAgent();
    bool GetDefualtDevice();
	bool IsLongmaiDev(const std::string &devID/*in*/, DeviceDetialInfo &info/*out*/);
    bool GetAppList(const std::string &devID);
	bool VerifyPINJudge();

private:
    WrapPkiAgent(QObject *parent = NULL);
    ~WrapPkiAgent();

    void InitConnection();
    bool ExtractLongmaiDev(const kpkiResp *resp);

    std::shared_ptr<UkeyVerifyWidget> m_verifyPtr;
	DeviceInfo m_defaultDevice;
	std::string m_defualtApp;
};

#endif // WRAPPKIAGENT_H
