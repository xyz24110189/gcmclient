#include "parsonjsonagent.h"
#include "parson.h"
#include <stdio.h>

ParsonJsonAgent::ParsonJsonAgent()
{

}

bool ParsonJsonAgent::ParsonDeviceBriefInfo(const std::string data/*in*/,
                                            DeviceBriefInfo &briefInfo/*out*/)
{
    JSON_Value *root_value = json_parse_string(data.c_str());
    if (!root_value) return false;

    JSON_Object *root_object = json_value_get_object(root_value);
    if (!root_object) return false;

    if (json_object_has_value(root_object, "devID"))
        briefInfo.devID = json_object_get_string(root_object, "devID");
    if (json_object_has_value(root_object, "devNumber"))
        briefInfo.devNumber = json_object_get_string(root_object, "devNumber");
    if (json_object_has_value(root_object, "devLable"))
        briefInfo.devLable = json_object_get_string(root_object, "devLable");
    if (json_object_has_value(root_object, "providerName"))
        briefInfo.providerName = json_object_get_string(root_object, "providerName");

    json_value_free(root_value);
    return true;
}


bool ParsonJsonAgent::ParsonDeviceList(const std::string &data/*in*/,
                                       deviceVecType &devVec/*out*/)
{
    JSON_Value *root_value = json_parse_string(data.c_str());
    if (!root_value) return false;

    JSON_Object *root_object = json_value_get_object(root_value);
    if (!root_object) return false;

    JSON_Array *device_array = json_object_get_array(root_object, "devices");
    if (!device_array) return false;

    for (int i = 0; i < json_array_get_count(device_array); ++i)
    {
		DeviceInfo data;
        JSON_Object *item_object = json_array_get_object(device_array, i);
        if (!item_object) continue;

        if (json_object_has_value(item_object, "devID"))
            data.devID = json_object_get_string(item_object, "devID");
        if (json_object_has_value(item_object, "devName"))
            data.devName = json_object_get_string(item_object, "devName");
        if (json_object_has_value(item_object, "devNumber"))
            data.devNumber = json_object_get_string(item_object, "devNumber");
        if (json_object_has_value(item_object, "devLable"))
            data.devLable = json_object_get_string(item_object, "devLable");
        if (json_object_has_value(item_object, "devType"))
            data.devType = json_object_get_string(item_object, "devType");
        devVec.push_back(data);
    }
    json_value_free(root_value);
    return true;
}

bool ParsonJsonAgent::ParsonDeviceDetailInfo(const std::string &data/*in*/,
											 DeviceDetialInfo &devInfo/*out*/)
{
	JSON_Value *root_value = json_parse_string(data.c_str());
	if (!root_value) return false;

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return false;

	if (json_object_has_value(root_object, "manufacturer"))
		devInfo.manufacturer = json_object_get_string(root_object, "manufacturer");
	if (json_object_has_value(root_object, "issuer"))
		devInfo.issuer = json_object_get_string(root_object, "issuer");
	if (json_object_has_value(root_object, "label"))
		devInfo.label = json_object_get_string(root_object, "label");
	if (json_object_has_value(root_object, "serialNumber"))
		devInfo.serialNumber = json_object_get_string(root_object, "serialNumber");
	if (json_object_has_value(root_object, "algSymCap"))
		devInfo.algSymCap = json_object_get_string(root_object, "algSymCap");
	if (json_object_has_value(root_object, "algAsymCap"))
		devInfo.algAsymCap = json_object_get_string(root_object, "algAsymCap");
	if (json_object_has_value(root_object, "algHashCap"))
		devInfo.algHashCap = json_object_get_string(root_object, "algHashCap");
	if (json_object_has_value(root_object, "devAuthAlgId"))
		devInfo.devAuthAlgId = json_object_get_string(root_object, "devAuthAlgId");
	if (json_object_has_value(root_object, "totalSpace"))
		devInfo.totalSpace = json_object_get_string(root_object, "totalSpace");
	if (json_object_has_value(root_object, "freeSpace"))
		devInfo.freeSpace = json_object_get_string(root_object, "freeSpace");
	if (json_object_has_value(root_object, "reserved"))
		devInfo.reserved = json_object_get_string(root_object, "reserved");

	if (json_object_has_value(root_object, "HWversion"))
	{
		JSON_Object *hw_object = json_object_get_object(root_object, "HWversion");
		if (hw_object)
		{
			if (json_object_has_value(hw_object, "major"))
				devInfo.hw_major = json_object_get_string(hw_object, "major");
			if (json_object_has_value(hw_object, "minor"))
				devInfo.hw_minor = json_object_get_string(hw_object, "minor");
		}
	}

	if (json_object_has_value(root_object, "firmwareVersion"))
	{
		JSON_Object *fw_object = json_object_get_object(root_object, "firmwareVersion");
		if (fw_object)
		{
			if (json_object_has_value(fw_object, "major"))
				devInfo.fw_major = json_object_get_string(fw_object, "major");
			if (json_object_has_value(fw_object, "minor"))
				devInfo.fw_minor = json_object_get_string(fw_object, "minor");
		}
	}
		
	json_value_free(root_value);
    return true;
}

bool ParsonJsonAgent::ParsonAppList(const std::string &data/*in*/,
                                    appVecType &appVec/*out*/)
{
    JSON_Value *root_value = json_parse_string(data.c_str());
    if (!root_value) return false;

    JSON_Object *root_object = json_value_get_object(root_value);
    if (!root_object) return false;

    JSON_Array *app_array = json_object_get_array(root_object, "apps");
    if (!app_array) return false;

    for (int i = 0; i < json_array_get_count(app_array); ++i)
    {
        appVec.push_back(json_array_get_string(app_array, i));
    }

    json_value_free(root_value);
	return true;
}

bool ParsonJsonAgent::ParsonVerifyPINRes(const std::string &data/*in*/,
										 VerifyPinInfo &info/*out*/)
{
	JSON_Value *root_value = json_parse_string(data.c_str());
	if (!root_value) return false;

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return false;
	
	if (json_object_has_value(root_object, "devID"))
		info.devID   = json_object_get_string(root_object, "devID");
	if (json_object_has_value(root_object, "appName"))
		info.appName = json_object_get_string(root_object, "appName");
	if (json_object_has_value(root_object, "PINType"))
		info.PINType = json_object_get_string(root_object, "PINType");
	if (json_object_has_value(root_object, "msg"))
		info.msg     = json_object_get_string(root_object, "msg");
	if (json_object_has_value(root_object, "pulRetryCount"))
		info.pulRetryCount = json_object_get_string(root_object, "pulRetryCount");

	json_value_free(root_value);
	return true;
}

bool ParsonJsonAgent::ParseCertRes(const std::string &data/*in*/,
                                   std::string &certResult/*out*/)
{
    JSON_Value *root_value = json_parse_string(data.c_str());
    if (!root_value) return false;

    JSON_Object *root_object = json_value_get_object(root_value);
    if (!root_object) return false;

    if (!json_object_has_value(root_object, "b64cert")) return false;

    certResult = json_object_get_string(root_object, "b64cert");

    json_value_free(root_value);
    return !certResult.empty();
}

bool ParsonJsonAgent::ParseSignDatResp(const std::string &data/*in*/,
                                       std::string &signResult/*out*/)
{
    JSON_Value *root_value = json_parse_string(data.c_str());
    if (!root_value) return false;

    JSON_Object *root_object = json_value_get_object(root_value);
    if (!root_object) return false;

    if (!json_object_has_value(root_object, "b64signData")) return false;

    signResult = json_object_get_string(root_object, "b64signData");

    json_value_free(root_value);
    return !signResult.empty();
}

bool ParsonJsonAgent::ParsePkcs7SignRes(const std::string &data/*in*/,
                                        std::string &signResult/*out*/)
{
    JSON_Value *root_value = json_parse_string(data.c_str());
    if (!root_value) return false;

    JSON_Object *root_object = json_value_get_object(root_value);
    if (!root_object) return false;

    if (!json_object_has_value(root_object, "signData")) return false;

    signResult = json_object_get_string(root_object, "signData");

    json_value_free(root_value);
    return !signResult.empty();
}

bool ParsonJsonAgent::ParseMbFromCert(const std::string &data/*in*/,
                                      std::string &strMb/*out*/)
{
    JSON_Value *root_value = json_parse_string(data.c_str());
    if (!root_value) return false;

    JSON_Object *root_object = json_value_get_object(root_value);
    if (!root_object) return false;

    if (!json_object_has_value(root_object, "subject")) return false;
    JSON_Object *suject_object = json_object_get_object(root_object, "subject");

    if (!json_object_has_value(suject_object, "ST")) return false;
    strMb = json_object_get_string(suject_object, "ST");

    json_value_free(root_value);
    return !strMb.empty();
}

std::string ParsonJsonAgent::BuildVerifyPINReq(const std::string &devID/*in*/,
                                               const std::string &appName/*in*/,
                                               const std::string &PINType/*in*/,
                                               const std::string &PIN/*in*/)
{
    char *serialized_string = NULL;
    std::string strJson;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "devID", devID.c_str());
    json_object_set_string(root_object, "appName", appName.c_str());
    json_object_set_string(root_object, "PINType", PINType.c_str());
    json_object_set_string(root_object, "PIN", PIN.c_str());
    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}

std::string ParsonJsonAgent::BuildGetDevInfoReq(const std::string &devID/*in*/)
{
	return BuildGetAppListReq(devID);
}

std::string ParsonJsonAgent::BuildGetAppListReq(const std::string &devID)
{
    char *serialized_string = NULL;
    std::string strJson;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "devID", devID.c_str());
    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}

std::string ParsonJsonAgent::BuildUnlockPINReq(const std::string &devId/*in*/,
                                               const std::string &appName/*in*/,
                                               const std::string &adminPIN/*in*/,
                                               const std::string &userPIN/*in*/)
{
    char *serialized_string = NULL;
    std::string strJson;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "devID", devId.c_str());
    json_object_set_string(root_object, "appName", appName.c_str());
    json_object_set_string(root_object, "adminPIN", adminPIN.c_str());
    json_object_set_string(root_object, "userPIN", userPIN.c_str());
    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}

std::string ParsonJsonAgent::BuildMakePkcs10Req(const std::string &devId/*in*/,
                                                const std::string &appName/*in*/,
                                                const std::string &conName/*in*/,
                                                const std::string &dn/*in*/,
                                                const int &extensionType/*in*/)
{
    char externType[8] = {0};
    char *serialized_string = NULL;
    std::string strJson;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "devID", devId.c_str());
    json_object_set_string(root_object, "appName", appName.c_str());
    json_object_set_string(root_object, "conName", conName.c_str());
    json_object_set_string(root_object, "dn", dn.c_str());

    sprintf(externType, "%d", extensionType);
    json_object_set_string(root_object, "extType", externType);
    json_object_set_string(root_object, "digestType", "2"); ///公钥的摘要值获取类型，默认为2。可选项1: md5，2：sha1，3：sm3，4：sha256
    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}

std::string ParsonJsonAgent::BuildInstallCertReq(const std::string & devId/*in*/,
                                                 const std::string & appName/*in*/,
                                                 const std::string & conName/*in*/,
                                                 const std::string & b64cert/*in*/,
                                                 const std::string & purpose/*in*/)
{
    char *serialized_string = NULL;
    std::string strJson;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "devID", devId.c_str());
    json_object_set_string(root_object, "appName", appName.c_str());
    json_object_set_string(root_object, "conName", conName.c_str());
    json_object_set_string(root_object, "b64cert", b64cert.c_str());
    json_object_set_string(root_object, "purpose", purpose.c_str());
    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}


std::string ParsonJsonAgent::BuildDelContainerReq(const std::string &devId/*in*/,
                                                  const std::string &appName/*in*/,
                                                  const std::string &containerName/*in*/)
{
    char *serialized_string = NULL;
    std::string strJson;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "devID", devId.c_str());
    json_object_set_string(root_object, "appName", appName.c_str());
    json_object_set_string(root_object, "containerName", containerName.c_str());
    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}

std::string ParsonJsonAgent::BuildCreateContainerReq(const std::string &devId/*in*/,
                                                     const std::string &appName/*in*/,
                                                     const std::string &containerName/*in*/)
{
    return ParsonJsonAgent::BuildDelContainerReq(devId, appName, containerName);
}

std::string ParsonJsonAgent::BuildGetContainersReq(const std::string &devId/*in*/,
												   const std::string &appName/*in*/)
{
	return ParsonJsonAgent::BuildDelContainerReq(devId, appName, "");
}

std::string ParsonJsonAgent::BuildGenb64KeypairReq(const std::string &devId,
                                                   const std::string &appName,
                                                   const std::string &conName,
                                                   const std::string &keyType,
                                                   const std::string &keyLen,
                                                   const int &purpose)
{
    char purposeBuf[8] = {0};
    char *serialized_string = NULL;
    std::string strJson;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "devID", devId.c_str());
    json_object_set_string(root_object, "appName", appName.c_str());
    json_object_set_string(root_object, "conName", conName.c_str());
    json_object_set_string(root_object, "keyType", keyType.c_str());
    json_object_set_string(root_object, "keyLen", keyLen.c_str());

    sprintf(purposeBuf, "%d", purpose);
    json_object_set_string(root_object, "purpose", purposeBuf);
    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}

std::string ParsonJsonAgent::BuildGetb64certReq(const std::string &devId/*in*/,
                                                const std::string &appName/*in*/,
                                                const std::string &conName/*in*/,
                                                const std::string &certType/*in*/)
{
    char *serialized_string = NULL;
    std::string strJson;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "devID", devId.c_str());
    json_object_set_string(root_object, "appName", appName.c_str());
    json_object_set_string(root_object, "conName", conName.c_str());
    json_object_set_string(root_object, "certType", certType.c_str());

    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}

std::string ParsonJsonAgent::BuildSignDataReq(const std::string &devId/*in*/,
                                              const std::string &appName/*in*/,
                                              const std::string &conName/*in*/,
                                              const std::string &srcData/*in*/,
                                              const unsigned int &type/*in*/)
{
    char typeBuf[8] = {0};
    char *serialized_string = NULL;
    std::string strJson;

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "devID", devId.c_str());
    json_object_set_string(root_object, "appName", appName.c_str());
    json_object_set_string(root_object, "conName", conName.c_str());
    json_object_set_string(root_object, "srcData", srcData.c_str());

    sprintf(typeBuf, "%d", type);
    json_object_set_string(root_object, "type", typeBuf);

    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}

std::string ParsonJsonAgent::BuildPkcs7SignReq(const std::string &devId/*in*/,
                                               const std::string &appName/*in*/,
                                               const std::string &conName/*in*/,
                                               const std::string &srcData/*in*/,
                                               const unsigned int &mdType/*in*/,
                                               const unsigned int &attachData/*in*/)
{
    char mdTypeBuf[8] = {0};
    char attachBuf[8] = {0};
    char *serialized_string = NULL;
    std::string strJson;

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "devID", devId.c_str());
    json_object_set_string(root_object, "appName", appName.c_str());
    json_object_set_string(root_object, "conName", conName.c_str());
    json_object_set_string(root_object, "srcData", srcData.c_str());

    sprintf(attachBuf, "%d", attachData);
    json_object_set_string(root_object, "attachData", attachBuf);

    sprintf(mdTypeBuf, "%d", mdType);
    json_object_set_string(root_object, "mdType", mdTypeBuf);

    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}

std::string ParsonJsonAgent::BuildCertParseReq(const std::string &cert/*in*/)
{
    char *serialized_string = NULL;
    std::string strJson;

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "cert", cert.c_str());

    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return strJson;
}

bool ParsonJsonAgent::ParsonCertResBody(const std::string &reqBody/*in*/, CertRequestInfo &info/*out*/)
{
	JSON_Value *root_value = json_parse_string(reqBody.data());
	if (!root_value) return false;

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return false;

	JSON_Object *body_object = json_object_get_object(root_object, "biz_body");
	if (!body_object) return false;

	if (json_object_has_value(body_object, "cert_dn"))
		info.dn = json_object_get_string(body_object, "cert_dn");
	if (json_object_has_value(body_object, "cert_algorithm"))
		info.algorithm = json_object_get_string(body_object, "cert_algorithm");
	if (json_object_has_value(body_object, "cert_pub_key_length"))
		info.pubKeyLength = json_object_get_string(body_object, "cert_pub_key_length");

	json_value_free(root_value);
	return true;
}

bool ParsonJsonAgent::ParsonP10Content(const std::string &resBody/*in*/, Pkcs10Info &info/*out*/)
{
	JSON_Value *root_value = json_parse_string(resBody.data());
	if (!root_value) return false;

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return false;

	if (json_object_has_value(root_object, "csrBase64"))
		info.csrBase64 = json_object_get_string(root_object, "csrBase64");
	if (json_object_has_value(root_object, "pubKeyDigest"))
		info.pubKeyDigest = json_object_get_string(root_object, "pubKeyDigest");

	json_value_free(root_value);
	return true;
}

bool ParsonJsonAgent::ParsonReqCertInfo(const std::string &reqBody/*in*/, CertInfo &info/*out*/)
{
	JSON_Value *root_value = json_parse_string(reqBody.data());
	if (!root_value) return false;

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return false;

	JSON_Object *body_object = json_object_get_object(root_object, "biz_body");
	if (!body_object) return false;

	if (json_object_has_value(body_object, "cert_sign"))
		info.certSign = json_object_get_string(body_object, "cert_sign");
	if (json_object_has_value(body_object, "cert_enc"))
		info.certEnc = json_object_get_string(body_object, "cert_enc");
	if (json_object_has_value(body_object, "cert_enc_key"))
		info.certEncKey = json_object_get_string(body_object, "cert_enc_key");
	if (json_object_has_value(body_object, "media_container"))
		info.mediaContainer = json_object_get_string(body_object, "media_container");
	if (json_object_has_value(body_object, "media_app"))
		info.mediaApp = json_object_get_string(body_object, "media_app");

	json_value_free(root_value);
	return true;
}

bool ParsonJsonAgent::ParsonCertSnVector(const std::string &reqBody/*in*/, std::vector<std::string> &certSnVec)
{
	JSON_Value *root_value = json_parse_string(reqBody.data());
	if (!root_value) return false;

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return false;

	JSON_Object *body_object = json_object_get_object(root_object, "biz_body");
	if (!body_object) return false;

	JSON_Array *certSn_array = json_object_get_array(body_object, "cert_sn");
	if (!certSn_array) return false;

	for (int i = 0; i < json_array_get_count(certSn_array); ++i)
	{
		std::string certSn = json_array_get_string(certSn_array, i);
		certSnVec.push_back(certSn);
	}

	json_value_free(root_value);
	return true;
}

bool ParsonJsonAgent::ParsonContainerVector(const std::string &resBody/*in*/, std::vector<std::string> &containerVec)
{
	JSON_Value *root_value = json_parse_string(resBody.data());
	if (!root_value) return false;
	
	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return false;

	JSON_Array *con_array = json_object_get_array(root_object, "containers");
	if (!con_array) return false;
	
	for (int i = 0; i < json_array_get_count(con_array); i++)
		containerVec.push_back(json_array_get_string(con_array, i));

	json_value_free(root_value);

	return true;
}

bool ParsonJsonAgent::ParsonAllCertVector(const std::string &resBody/*in*/, std::vector<AllCertInfo> &allCertVec)
{
	JSON_Value *root_value = json_parse_string(resBody.data());
	if (!root_value) return false;

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return false;

	JSON_Array *cert_array = json_object_get_array(root_object, "certs");
	if (!cert_array) return false;

	AllCertInfo info;
	for (int i = 0; i < json_array_get_count(cert_array); ++i)
	{
		JSON_Object *item_object = json_array_get_object(cert_array, i);
		if (json_object_has_value(item_object, "devID"))
			info.devID = json_object_get_string(item_object, "devID");
		if (json_object_has_value(item_object, "devName"))
			info.devName = json_object_get_string(item_object, "devName");
		if (json_object_has_value(item_object, "manufacturer"))
			info.manufacturer = json_object_get_string(item_object, "manufacturer");
		if (json_object_has_value(item_object, "appName"))
			info.appName = json_object_get_string(item_object, "appName");
		if (json_object_has_value(item_object, "containerName"))
			info.containerName = json_object_get_string(item_object, "containerName");
		if (json_object_has_value(item_object, "hasPIN"))
			info.hasPIN = json_object_get_string(item_object, "hasPIN");
		if (json_object_has_value(item_object, "hasFinger"))
			info.hasFinger = json_object_get_string(item_object, "hasFinger");

		JSON_Object *userCN_object = json_object_get_object(item_object, "subjectName");
		if (userCN_object)
			if (json_object_has_value(userCN_object, "CN"))
				info.userCN = json_object_get_string(userCN_object, "CN");

		JSON_Object *issuerCN_object = json_object_get_object(item_object, "subjectName");
		if (issuerCN_object)
			if (json_object_has_value(issuerCN_object, "CN"))
				info.issuerCN = json_object_get_string(issuerCN_object, "CN");

		if (json_object_has_value(item_object, "SN"))
			info.SN = json_object_get_string(item_object, "SN");
		if (json_object_has_value(item_object, "actionDate"))
			info.actionDate = json_object_get_string(item_object, "actionDate");
		if (json_object_has_value(item_object, "validDate"))
			info.validDate = json_object_get_string(item_object, "validDate");
		if (json_object_has_value(item_object, "certType"))
			info.certType = json_object_get_string(item_object, "certType");

		allCertVec.push_back(info);
	}

	json_value_free(root_value);
    return true;
}

bool ParsonJsonAgent::ParsonUnloclResBody(const std::string &reqBody/*in*/, UnLockPin &unlockInfo)
{
	JSON_Value *root_value = json_parse_string(reqBody.data());
	if (!root_value) return false;

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return false;

	JSON_Object *body_object = json_object_get_object(root_object, "biz_body");
	if (!body_object) return false;

	if (json_object_has_value(body_object, "so_pin"))
		unlockInfo.adminPIN = json_object_get_string(body_object, "so_pin");
	if (json_object_has_value(body_object, "user_pin"))
		unlockInfo.newUserPIN = json_object_get_string(body_object, "user_pin");

	json_value_free(root_value);
    return true;
}

bool ParsonJsonAgent::ParsonSSOResBody(const std::string &resBody/*in*/, std::string &strData/*out*/)
{
    JSON_Value *root_value = json_parse_string(resBody.data());
    if (!root_value) return false;

    JSON_Object *root_object = json_value_get_object(root_value);
    if (!root_object) return false;

    if (!json_object_has_value(root_object, "code")) return false;

    int code = json_object_get_number(root_object, "code");
    if (code != 20000) return false;

    if (!json_object_has_value(root_object, "data")) return false;

    JSON_Value *val = json_object_get_value(root_object, "data");
    if (val != NULL && json_value_get_type(val) == json_value_type::JSONNull) return true;
    strData = json_object_get_string(root_object, "data");

    json_value_free(root_value);
    return !strData.empty();
}

bool ParsonJsonAgent::ParsonRandomResBody(const std::string &resBody/*in*/, std::string &strRandom/*out*/)
{
    ParsonSSOResBody(resBody, strRandom);
}

std::string ParsonJsonAgent::BuildEmptyResBody(const std::string &reqBody/*in*/)
{
	std::string strJson;
	char *serialized_string = NULL;

	JSON_Value *root_value = json_parse_string(reqBody.data());
	if (!root_value) return "";

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return "";

	JSON_Object *body_object = json_object_get_object(root_object, "biz_body");
	if (!body_object) return "";

	json_object_clear(body_object);

	serialized_string = json_serialize_to_string_pretty(root_value);
	strJson = serialized_string;
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);

	return strJson;
}

std::string ParsonJsonAgent::BuildErrorResBody(const std::string &reqBody/*in*/, ResErrCode errCode/*in*/,
											   const std::string &errMsg/*in*/, const std::string &errDetail/*in*/)
{
	std::string strJson;
	char *serialized_string = NULL;

	JSON_Value *root_value = json_parse_string(reqBody.data());
	if (!root_value) return "";

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return "";

	JSON_Object *body_object = json_object_get_object(root_object, "biz_body");
	if (!body_object) return "";

	json_object_clear(body_object);

	char errCodeBuff[32] = { 0 };
	snprintf(errCodeBuff, 32, "%d", errCode);
	json_object_set_string(body_object, "error_code", errCodeBuff);
	json_object_set_string(body_object, "error_message", errMsg.data());
	json_object_set_string(body_object, "error_detail", errDetail.data());

	serialized_string = json_serialize_to_string_pretty(root_value);
	strJson = serialized_string;
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);

	return strJson;
}

std::string ParsonJsonAgent::BuildCertResBody(const std::string &reqBody/*in*/, const Pkcs10Info &info/*in*/)
{
	std::string strJson;
	char *serialized_string = NULL;

	JSON_Value *root_value = json_parse_string(reqBody.data());
	if (!root_value) return "";

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return "";

	JSON_Object *body_object = json_object_get_object(root_object, "biz_body");
	if (!body_object) return "";

	json_object_clear(body_object);
	json_object_set_string(body_object, "cert_request", info.csrBase64.data());

	serialized_string = json_serialize_to_string_pretty(root_value);
	strJson = serialized_string;
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);

	return strJson;
}

std::string ParsonJsonAgent::BuildMediaInfoBody(const std::string &reqBody/*in*/, const DeviceInfo &info/*in*/)
{
	std::string strJson;
	char *serialized_string = NULL;

	JSON_Value *root_value = json_parse_string(reqBody.data());
	if (!root_value) return "";

	JSON_Object *root_object = json_value_get_object(root_value);
	if (!root_object) return "";

	JSON_Object *body_object = json_object_get_object(root_object, "biz_body");
	if (!body_object) return "";

	json_object_set_string(body_object, "media_sn", info.devNumber.data());
	json_object_set_string(body_object, "media_hd_type", "100");
	json_object_set_string(body_object, "media_manufacturer", info.detailInfo.manufacturer.data());
	json_object_set_string(body_object, "media_brand", info.detailInfo.issuer.data());

	serialized_string = json_serialize_to_string_pretty(root_value);
	strJson = serialized_string;
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);

	return strJson;
}

std::string ParsonJsonAgent::BuildSSOResBody(int code, const std::string &msg, const std::string &data)
{
    std::string strJson;
    char *serialized_string = NULL;

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_number(root_object, "code", code);
    json_object_set_string(root_object, "message", msg.c_str());
    json_object_set_string(root_object, "data", data.c_str());

    serialized_string = json_serialize_to_string_pretty(root_value);
    strJson = serialized_string;
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    return strJson;
}



















