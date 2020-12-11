#include "confighandle.h"
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtGui/QDesktopServices>
#include <QtCore/QDir>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QStandardPaths>
#endif

ConfigHandle::ConfigHandle()
{

}

ConfigHandle::~ConfigHandle()
{
	m_settingPtr.reset();
}

void ConfigHandle::InitConfig()
{
	QString dataPath;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	dataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
	dataPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

	dataPath += QDir::separator();
	dataPath += CONFIG_FILE_NAME;
	m_settingPtr = std::make_shared<QSettings>(dataPath, QSettings::IniFormat);
}

void ConfigHandle::SetRemoteServerConf(const std::string &Ip, unsigned short port)
{
	m_settingPtr->beginGroup("RemoteConfig");
	m_settingPtr->setValue("RemoteIp", Ip.data());
	m_settingPtr->setValue("RemotePort", port);
	m_settingPtr->endGroup();
}

void ConfigHandle::GetRemoteServerConf(std::string &Ip, unsigned short &port)
{
	m_settingPtr->beginGroup("RemoteConfig");
	Ip = m_settingPtr->value("RemoteIp", "127.0.0.1").toString().toLocal8Bit().data();
	port = m_settingPtr->value("RemotePort", "8080").toUInt();
	m_settingPtr->endGroup();
}

void ConfigHandle::SetLocalServerPort(unsigned short port)
{
	m_settingPtr->beginGroup("LocalConfig");
	m_settingPtr->setValue("LocalPort", port);
	m_settingPtr->endGroup();
}

void ConfigHandle::GetLocalServerPort(unsigned short &port)
{
	m_settingPtr->beginGroup("LocalConfig");
	port = m_settingPtr->value("LocalPort", "3150").toUInt();
	m_settingPtr->endGroup();
}
