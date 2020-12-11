#ifndef CONFIGHANDLE_H
#define CONFIGHANDLE_H
#include "commontype.h"
#include <memory>

class QSettings;
class ConfigHandle
{
public:
    ConfigHandle();
	~ConfigHandle();

	void InitConfig();
	void SetRemoteServerConf(const std::string &Ip, unsigned short port);
	void GetRemoteServerConf(std::string &Ip, unsigned short &port);

	void SetLocalServerPort(unsigned short port);
	void GetLocalServerPort(unsigned short &port);

private:
	std::shared_ptr<QSettings> m_settingPtr;
};

#endif // CONFIGHANDLE_H