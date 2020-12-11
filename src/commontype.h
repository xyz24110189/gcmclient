#ifndef COMMONTYPE_H
#define COMMONTYPE_H

#include <vector>
#include <string>
#include <stdint.h>
#include <plog/Log.h>

#define SERVER_CERT_FILE "/certs/cert.pem"
#define SERVER_PRIVATE_KEY_FILE "/certs/key.pem"

#define CA_CERT_FILE "/certs/ca-bundle.crt"

// 专有机配置文件
#define SYSTEM_INFO_FILE "/etc/.systeminfo"
#define SYSTEM_MACHINE_ID "/etc/machine-id"

#define DB_NAME     "uniqueAuth.db"

#define TRANSLATION_FILE "GCMClient_zh_CN.qm"

#define CONFIG_FILE_NAME "GCMConfig.ini"

#define CONTENT_TYPE "application/json;charset=utf-8"

#define M_SIZE 1024 * 1024

enum ResErrCode
{
    ERR_SUCCESS           = 0x00004E20, ///20000 成功
    ERR_CERT_IMPORT_ERROR = 0x00007530, ///30000 证书导入错误
	ERR_CERT_REQUST_ERROR,              ///证书请求错误
	ERR_CERT_DELETE_ERROR,              ///证书删除错误
    ERR_PIN_UNLOCK_ERROR,               ///解锁介质错误
    ERR_PARSON_PARA_ERROR = 0x00007597, ///参数解析失败
    ERR_KEY_NOT_EXIST     = 0x00007598, ///没有插入龙脉key
    ERR_GET_CERT_ERROR    = 0x00007599, ///获取证书失败
    ERR_GET_MB_ERROR      = 0x0000759A, ///获取密标信息失败
    ERR_SIGNATURE_FAILED  = 0x0000759B, ///签名失败
    ERR_GET_TGT_FAILED    = 0x0000759C, ///获取授权票据失败
    ERR_GET_ST_FAILED     = 0x0000759D, ///获取票据失败
    ERR_LOGOUT_FAILED     = 0x0000759E  ///注销会话失败
};

enum UkeyOperation
{
	INSERT    = 0x0FFF0001,             ///ukey 插入
	EXTRACT   = 0x0FFF0002              ///ukey 拔出
};

enum SignType
{
    PIN_TYPE = 1,
    UKEY_TYPE
};

enum SignP7Algorithm
{
    MD5 = 1,
    SHA1,
    SM3,
    SHA256
};

enum SignCustom
{
    PM_BD = 1,
    SM2_RSA,
    SSL_TYPE
};

struct DeviceBriefInfo {
    std::string devID;                ///设备ID
    std::string devNumber;            ///设备编号，设备自带
    std::string devLable;             ///设备标签
    std::string providerName;         ///提供商名称
};

struct DeviceDetialInfo {
	std::string manufacturer;         ///设备厂商信息
	std::string issuer;				  ///发行厂商
	std::string label;				  ///标签
	std::string serialNumber;		  ///序列号
	std::string hw_major;             ///设备硬件主版本
	std::string hw_minor;			  ///设备硬件次版本
	std::string fw_major;			  ///设备固件主版本
	std::string fw_minor;			  ///设备固件次版本
	std::string algSymCap;			  ///分组密码算法标识
	std::string algAsymCap;			  ///非对称密码算法标识
	std::string algHashCap;			  ///密码杂凑算法标识
	std::string devAuthAlgId;		  ///设备认证使用的分组密码算法标识
	std::string totalSpace;			  ///设备总空间大小
	std::string freeSpace;			  ///用户可用空间大小
	std::string reserved;			  ///保留扩展
};


struct DeviceInfo {
    std::string devID;                ///设备ID
    std::string devName;              ///设备名称
    std::string devNumber;            ///设备编号，设备自带
    std::string devLable;             ///设备标签
    std::string devType;              ///设备类型
	std::string defaultContent;       ///默认容器
	DeviceDetialInfo detailInfo;      ///设备详细信息
    std::vector<std::string> appVec;  ///app列表
};

struct VerifyPinInfo {
	std::string devID;          ///设备ID
	std::string appName;        ///应用名称
	std::string PINType;        ///PIN类型
	std::string pulRetryCount;  ///错误后返回的剩余重试次数
	std::string msg;			///错误提示消息
};

struct CertRequestInfo {
	std::string dn;             ///实体标识
	std::string algorithm;      ///证书算法 
	std::string pubKeyLength;   ///证书公钥长度
};

struct Pkcs10Info {
	std::string csrBase64;      ///p10内容(base64编码)
	std::string pubKeyDigest;   ///公钥摘要值
};

struct CertInfo {
	std::string certSign;        ///签名证书
	std::string certEnc;         ///加密证书
	std::string certEncKey;      ///加密密钥
	std::string mediaContainer;  ///容器名
	std::string mediaApp;        ///应用名
};

struct AllCertInfo {
	std::string devID;           ///设备ID
	std::string devName;         ///设备名称
	std::string manufacturer;    ///设备厂商
	std::string appName;         ///应用名称
	std::string containerName;   ///容器名称
	std::string hasPIN;          ///是否有PIN码
	std::string hasFinger;       ///是否具有指纹
	std::string signFlag;        ///证书用途, 1表示签名证书, 0表示加密证书
	std::string userCN;          ///使用者CN项
	std::string issuerCN;        ///颁发者CN项
	std::string SN;              ///序列号
	std::string actionDate;      ///生效时间
	std::string validDate;       ///失效时间
	std::string certType;        ///证书类型
};

struct UnLockPin {
	std::string adminPIN;      ///管理员PIN码
	std::string newUserPIN;    ///用户PIN码
};

struct AppData {
    SignType type;
    std::string secMark;
    std::string userName;
    std::string password;
    std::string appSign;
};

struct GcmSysInfo {
    std::string productName;
    std::string productModel;
    std::string shelterModel;
    std::string ID;
    std::string producer;
    std::string name;
    std::string release;
    std::string kernel;
    std::string bit;
    std::string IO;
    std::string version;
    std::string biosVersion;
    std::string CPU;
    std::string memory;
    std::string hdSerial;
    std::string hdCapacity;
    std::string mainBoard;
};

#endif // COMMONTYPE_H
