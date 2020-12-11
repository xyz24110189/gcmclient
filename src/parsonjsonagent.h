#ifndef PARSONJSONAGENT_H
#define PARSONJSONAGENT_H
#include <string>
#include <vector>
#include <map>
#include "commontype.h"

class ParsonJsonAgent
{
public:
    typedef std::vector<DeviceInfo> deviceVecType;
    typedef std::vector<std::string> appVecType;

    ParsonJsonAgent();

    ///内部接口函数
    static bool ParsonDeviceBriefInfo(const std::string data/*in*/,
                                      DeviceBriefInfo &briefInfo/*out*/);

    static bool ParsonDeviceList(const std::string &data/*in*/,
                                 deviceVecType &devVec/*out*/);

	static bool ParsonDeviceDetailInfo(const std::string &data/*in*/,
								       DeviceDetialInfo &devInfo/*out*/);

    static bool ParsonAppList(const std::string &data/*in*/,
                              appVecType &appVec/*out*/);

	static bool ParsonVerifyPINRes(const std::string &data/*in*/,
								   VerifyPinInfo &info/*out*/);

    static bool ParseCertRes(const std::string &data/*in*/,
                             std::string &certResult/*out*/);

    static bool ParseSignDatResp(const std::string &data/*in*/,
                                 std::string &signResult/*out*/);

    static bool ParsePkcs7SignRes(const std::string &data/*in*/,
                                  std::string &signResult/*out*/);

    static bool ParseMbFromCert(const std::string &data/*in*/,
                                  std::string &strMb/*out*/);

    static std::string BuildVerifyPINReq(const std::string &devID/*in*/,
                                         const std::string &appName/*in*/,
                                         const std::string &PINType/*in*/,
                                         const std::string &PIN/*in*/);

	static std::string BuildGetDevInfoReq(const std::string &devID/*in*/);

    static std::string BuildGetAppListReq(const std::string &devID/*in*/);

    static std::string BuildUnlockPINReq(const std::string &devId/*in*/,
                                         const std::string &appName/*in*/,
                                         const std::string &adminPIN/*in*/,
                                         const std::string &userPIN/*in*/);

    static std::string BuildMakePkcs10Req(const std::string &devId/*in*/,
                                          const std::string &appName/*in*/,
                                          const std::string &conName/*in*/,
                                          const std::string &dn/*in*/,
                                          const int &extensionType/*in*/);

    static std::string BuildInstallCertReq(const std::string & devId/*in*/,
                                           const std::string & appName/*in*/,
                                           const std::string & conName/*in*/,
                                           const std::string & b64cert/*in*/,
                                           const std::string & purpose/*in*/);

    static std::string BuildDelContainerReq(const std::string &devId/*in*/,
                                            const std::string &appName/*in*/,
                                            const std::string &containerName/*in*/);

    static std::string BuildCreateContainerReq(const std::string &devId/*in*/,
                                               const std::string &appName/*in*/,
                                               const std::string &containerName/*in*/);

	static std::string BuildGetContainersReq(const std::string &devId/*in*/, 
											 const std::string &appName/*in*/);


    static std::string BuildGenb64KeypairReq(const std::string &devId/*in*/,
                                             const std::string &appName/*in*/,
                                             const std::string &conName/*in*/,
                                             const std::string &keyType/*in*/,
                                             const std::string &keyLen/*in*/,
                                             const int &purpose/*in*/);

    static std::string BuildGetb64certReq(const std::string &devId/*in*/,
                                          const std::string &appName/*in*/,
                                          const std::string &conName/*in*/,
                                          const std::string &certType/*in*/);

    static std::string BuildSignDataReq(const std::string &devId/*in*/,
                                        const std::string &appName/*in*/,
                                        const std::string &conName/*in*/,
                                        const std::string &srcData/*in*/,
                                        const unsigned int &type/*in*/);

    static std::string BuildPkcs7SignReq(const std::string &devId/*in*/,
                                         const std::string &appName/*in*/,
                                         const std::string &conName/*in*/,
                                         const std::string &srcData/*in*/,
                                         const unsigned int &mdType/*in*/,
                                         const unsigned int &attachData/*in*/);

    static std::string BuildCertParseReq(const std::string &cert/*in*/);

    ///外部接口函数
	static bool ParsonCertResBody(const std::string &reqBody/*in*/, CertRequestInfo &info/*out*/);
	static bool ParsonP10Content(const std::string &resBody/*in*/, Pkcs10Info &info/*out*/);
	static bool ParsonReqCertInfo(const std::string &reqBody/*in*/, CertInfo &info/*out*/);
	static bool ParsonCertSnVector(const std::string &reqBody/*in*/, std::vector<std::string> &certSnVec);
	static bool ParsonContainerVector(const std::string &resBody/*in*/, std::vector<std::string> &containerVec);
	static bool ParsonAllCertVector(const std::string &resBody/*in*/, std::vector<AllCertInfo> &allCertVec);
	static bool ParsonUnloclResBody(const std::string &reqBody/*in*/, UnLockPin &unlockInfo);
    static bool ParsonSSOResBody(const std::string &resBody/*in*/, std::string &strData/*out*/);
    static bool ParsonRandomResBody(const std::string &resBody/*in*/, std::string &strRandom/*out*/);

	static std::string BuildEmptyResBody(const std::string &reqBody/*in*/);
	static std::string BuildErrorResBody(const std::string &reqBody/*in*/, ResErrCode errCode/*in*/,
										 const std::string &errMsg/*in*/, const std::string &errDetail/*in*/);

	static std::string BuildCertResBody(const std::string &reqBody/*in*/, const Pkcs10Info &info/*in*/);
	static std::string BuildMediaInfoBody(const std::string &reqBody/*in*/, const DeviceInfo &info/*in*/);

    static std::string BuildSSOResBody(int code, const std::string &msg, const std::string &data);
};

#endif // PARSONJSONAGENT_H
























