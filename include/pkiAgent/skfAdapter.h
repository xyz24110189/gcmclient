
#ifndef THIS_IS_PKIADAPTER_HEADER_20191210
#define THIS_IS_PKIADAPTER_HEADER_20191210

#include "agentCommon.h"

#define SAR_OK                      0x00000000    //成功
#define SAR_FAIL                    0x0A000001    //失败


#ifdef SKF_ECC_BLOB_DEFINE	//如果外部调用程序已经此结构体，则通过此宏定义来屏蔽掉此结构体

#define ECC_MAX_XCOORDINATE_BITS_LEN 512        //ECC算法X座标的最大长度
///ECC签名数据结构
typedef struct stECCSIGNATUREBLOB{
	unsigned char r[ECC_MAX_XCOORDINATE_BITS_LEN/8];
    unsigned char s[ECC_MAX_XCOORDINATE_BITS_LEN/8];
} ECCSIGNATUREBLOB, *PECCSIGNATUREBLOB;

#endif

/**
 * @param url 访问key的设备路径
 * @param pbData 摘要数据
 * @param ulDataLen 摘要数据长度
 * @param pbSignature 签名数据
 * @param pulSignLen 签名数据的长度
 * @return 0表示成功，非0表示失败
 */
EXPORT_PKIAGENT unsigned int rsaSignData(char *url, unsigned char *pbData, unsigned int ulDataLen, 
									unsigned char *pbSignature, unsigned int *pulSignLen);

/**
 * @param url 访问key的设备路径
 * @param pbData 摘要数据
 * @param ulDataLen 摘要数据长度
 * @param pSignature 签名数据
 * @return 0表示成功，非0表示失败
 */
EXPORT_PKIAGENT unsigned int eccSignData(char *url, unsigned char *pbData, unsigned int ulDataLen, PECCSIGNATUREBLOB pSignature);

#endif