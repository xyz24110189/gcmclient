/**
 * @brief PKI中间件AGENT的接口头文件
 * @author sharp
 * @email yangcp@koal.com
 */
#ifndef THIS_IS_PKIAGENT_4C_HEADER_201911261537
#define THIS_IS_PKIAGENT_4C_HEADER_201911261537

#include "agentCommon.h"
#include "byteArray.h"
#define  MAX_JSON_LEN 8192

/**
 * @brief PKI中间件请求结构体
 */ 
struct  kpkiReq
{
	/// 请求ID, 登录请求reqid = 0 , 登录成功后每次请求该字段递增
	int  reqid ;  
    /// 请求类型
    int   msgType; 
    /// 消息版本  body json模板的版本
    int   version; 
    /// 扩展 备用， 根据不同的msgType定义可能不一样
    int64 extend; 
    /// JSON字符串
    char jsonBody[MAX_JSON_LEN];
    /// 动态数组
    koal::CByteArray *data;

    kpkiReq() {
        reqid = 0;
        msgType = 0;
        version = 0;
        extend  = 0;
        memset(jsonBody, 0, sizeof(jsonBody));
        data = new koal::CByteArray();
    }
    ~kpkiReq() {
        if (data)
            delete data;
        data = NULL;
    }         
};

/**
 * @brief PKI中间件相应
 */
struct  kpkiResp {
    /// 异步请求的时候为reqAsync方法返回值
    int  respid ;
    /// 请求类型
    int  msgType; 
    /// 消息版本  body json模板的版本
    int  version;
    /// 错误码，0为执行成功，
    int  errCode ;
    /// 扩展码
    int64  extend; 
    /// json消息体, 如果errCode不为零，jsonbody = {"msg":"xxxxxxxxx"}
    char jsonBody[MAX_JSON_LEN]; 
    /// 动态数组
    koal::CByteArray *data;

    kpkiResp() {
        respid = 0;
        msgType = 0;
        version = 0;
        errCode = 0;
        extend = 0;
        data = new koal::CByteArray();
        memset(jsonBody, 0, sizeof(jsonBody));
    }
    ~kpkiResp() {
        if (data)
            delete data;
        data = NULL;
    }           
};

/**
 * 服务分类
 */
enum PKI_SERVICE {
    ENROLLSERVICE,
    DEVSERVICE,
	SIGNXSERVICE,
	KMAILSERVICE
};

/**
 * 服务消息定义
 * kpkiReq的msgType字段赋值
 */ 
enum MSG_ENROLL {
    ///创建PKCS10 请求
    MSG_ENROLL_MKP10=0x20,
	///生成密钥对
    MSG_ENROLL_KEYPAIR=0x21,
	///导入密钥对
    MSG_ENROLL_IMPORTKEYPAIR=0x22,
	///导入证书
    MSG_ENROLL_IMPORTX509=0x23,
	///导入pfx证书
    MSG_ENROLL_IMPORTPFX=0x24,
	///导出证书
    MSG_ENROLL_GETCERT=0x25,
};

enum MSG_DEVICE {
    ///获取设备列表
    MSG_DEVICE_GETDEVICES=0x10,
    ///获取设备信息    
    MSG_DEVICE_GETDEVINFO=0x11,
    ///设置设备标签
    MSG_DEVICE_SETDEVLABLE=0x12,
    ///设备命令传输
    MSG_DEVICE_TRANSMITDATA=0x13,
    ///设备认证
    MSG_DEVICE_DEVAUTH=0x14,
    ///修改设备认证密钥
    MSG_DEVICE_CHANGEAUTHKEY=0x15,
    ///获取PIN
    MSG_DEVICE_GETPININFO=0x16,
    ///修改PIN
    MSG_DEVICE_CHANGEPIN=0x17,
    ///校验PIN
    MSG_DEVICE_VERIFYPIN=0x18,
    ///解锁PIN码
    MSG_DEVICE_UNLOCKPIN=0x19,
    ///获取应用列表
    MSG_DEVICE_GETAPPLIST=0x1a,
    ///创建应用
    MSG_DEVICE_CREATEAPP=0x1b,
    ///删除应用
    MSG_DEVICE_DELAPP=0x1c,
    ///获取容器列表
    MSG_DEVICE_GETCONTAINERS=0x1d,
    ///创建容器
    MSG_DEVICE_CREATECONTAINER=0x1e,
    ///删除容器
    MSG_DEVICE_DELCONTAINER=0x1f,
    ///获得容器类型
    MSG_DEVICE_GETCONTAINERTYPE=0x20,
    ///导入数字证书
    MSG_DEVICE_IMPORTCERTIFICATE=0x21,
    ///导出数字证书
    MSG_DEVICE_EXPORTCERTIFICATE=0x22,   
    ///导出公钥
    MSG_DEVICE_EXPORTPUBLICKEY=0x23,
    ///获取Provider列表
    MSG_DEVICE_GETPROVIDERS=0x24,
    ///设置Provider
    MSG_DEVICE_SETPROVIDER=0x25,
	///外来公钥加密
	MSG_DEVICE_EXTPUBKEYENCRYPT=0x26,
	///外来私钥解密
	MSG_DEVICE_EXTPRIKEYDECRYPT=0x27,
	///获取证书列表
	MSG_DEVICE_GETALLCERT=0x28,
	///指纹初始化
	MSG_DEVICE_INITFINGER=0x30,
	///是否存在指纹
	MSG_DEVICE_HASFINGER=0x31,
	///验证指纹
	MSG_DEVICE_VERIFYFINGER=0x32,
	///取消指纹
	MSG_DEVICE_CANCLEFINGER=0x33
};

enum MSG_SIGNX {
    ///数据签名
    MSG_SIGNX_SIGNDATA=0x10,
    ///验证签名
    MSG_SIGNX_VERIFYSIGN=0x11,
    ///数据签名P7
    MSG_SIGNX_SIGNP7=0x12,
    ///验证签名P7
    MSG_SIGNX_VERIFYSIGNP7=0x13,
    ///外部公钥ECC验签
    MSG_SIGNX_EXTECCPUBVERIFY=0x14,
    ///外部证书ECC验签
    MSG_SIGNX_EXTECCCERTVERIFY=0x15,
    ///根据模板生成证书
    MSG_SIGNX_MKCERTFROMTEMP=0x16,
    ///解析证书
    MSG_SIGNX_PARSECERT=0x17,
    ///构造P7数字信封
    MSG_SIGNX_ENVELOPEENC=0x18,
    ///解析P7数字信封
    MSG_SIGNX_ENVELOPEDEC=0x19,
    ///P7外部证书验签
    MSG_SIGNX_EXTECCCERTVERIFYP7=0x1A,
    ///获取证书拓展项
    MSG_SIGNX_GETEXTENSION=0x1B
};

enum MSG_KMAIL {
	/// 设置文本主体
	MSG_KMAIL_SETTEXTBODY = 0x10,
	/// 获取文本主体
	MSG_KMAIL_GETTEXTBODY = 0x11,
	/// 设置超文本主体
	MSG_KMAIL_SETHTMLBODY = 0x12,
	/// 获取超文本主体
	MSG_KMAIL_GETHTMLBODY = 0x13,
	/// 邮件内容组合
	MSG_KMAIL_COMPOSE = 0x14,
	/// 获取指定索引的数据包
	MSG_KMAIL_GETCOMPOSEDDATA = 0x15,
	/// 准备解析邮件内容
	MSG_KMAIL_PREPAREPARSE = 0x16,
	/// 解析邮件内容
	MSG_KMAIL_PARSE = 0x17,
	/// 清除邮件相关信息
	MSG_KMAIL_CLEAR = 0x18,
	/// 添加附件
	MSG_KMAIL_ADDATTACHFILE = 0x19,
	/// 获取附件数量
	MSG_KMAIL_GETATTACHCOUNT = 0x1A,
	/// 获取指定附件信息
	MSG_KMAIL_GETATTACHFILEINFO = 0x1B,
	/// 指定附件另存为
	MSG_KMAIL_DOATTACHSAVEAS = 0x1C,
	/// 获取指定附件的指定扩展字段
	MSG_KMAIL_GETATTACHFIELDINFO = 0x1D,
	/// 设置邮件类型
	MSG_KMAIL_SETMAILTYPE = 0x1E,
	/// 获取邮件类型
	MSG_KMAIL_GETMAILTYPE = 0x1F,
	/// 设置加密证书
	MSG_KMAIL_SETENCCERTS = 0x20,
	/// 获取邮件中的签名证书
	MSG_KMAIL_GETMAILSIGNER = 0x21,
	/// 获取指定容器的签名证书
	MSG_KMAIL_GETSIGNCERT = 0x22,
	/// 获取证书中的指定项的值
	MSG_KMAIL_GETCERTITEM = 0x23,
	/// 签名
	MSG_KMAIL_SIGNDATA = 0x24
};

///回调方法指针
typedef bool (*agentNotify)(const kpkiResp * pResp, void * pUserData);
typedef bool (*respCallBack)(const PKI_SERVICE * pSvc,
							const kpkiResp * pResp, 
                            void * pUserData);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///导出接口
/**
 * @brief 初始化Agent
 * @param pcb  推送的回调方法
 * @return true 表示成功， false表示失败
 */
EXPORT_PKIAGENT bool createAgent(agentNotify pfunc, respCallBack cb, void * pUserData = NULL);

/**
 * @brief 应用登录
 * @param pAppName  应用名称
 * @param pAppID    应用ID
 * @param pToken    应用TOKEN
 * @return true 表示登录请求发送成功， false登录请求发送失败
 */ 
EXPORT_PKIAGENT bool loginAgent(const char * pAppName, const char * pAppID ,const char * pToken );

/**
 * @brief 登出
 */ 
EXPORT_PKIAGENT bool logoutAgent();


/**
 * @brief 同步请求
 * @param svc 服务的类型 
 * @param pReq 请求数据
 * @param pResp 返回数据
 * @return true 表示同步请求发送成功， false同步请求发送失败
 */ 
EXPORT_PKIAGENT bool reqSync(PKI_SERVICE svc, const kpkiReq * pReq , kpkiResp * pResp); 


/**
 * @brief 异步请求
 * @param svc 服务的类型 
 * @param pReq 请求数据
 * @param pUserData 为用户自定义数据,在回调接口里面会传入
 * @return 0标识发送异步请求失败，非0表示发送异步请求成功，返回值为resp.respid
 */
EXPORT_PKIAGENT int reqAsync(PKI_SERVICE svc, const kpkiReq * pReq, 
                      void * pUserData);

/**
 * 释放代理
 */ 
EXPORT_PKIAGENT void releaseAgent();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif