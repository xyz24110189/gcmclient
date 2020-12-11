#include "ukeysystemtray.h"
#include "settingdialog.h"
#include "commontype.h"
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QSystemTrayIcon>

UkeySystemTrayIcon::UkeySystemTrayIcon()
{
	CreateActions();
	CreateTrayIcon();
}

UkeySystemTrayIcon::~UkeySystemTrayIcon()
{

}

void UkeySystemTrayIcon::Show()
{
	m_trayIcon->show();
}

void UkeySystemTrayIcon::OnAuthChange(bool auth)
{
	if (auth)
		m_trayIcon->setIcon(QIcon(":/images/tray.png"));
	else
		m_trayIcon->setIcon(QIcon(":/images/tray_noAuth.png"));
}

void UkeySystemTrayIcon::OnLocalActionTrigger()
{
	LocalSettingDialog dialog;
	dialog.Show();
}

void UkeySystemTrayIcon::OnServerActionTrigger()
{
	ServerSettingDialog dialog;
	dialog.Show();
}

void UkeySystemTrayIcon::CreateActions()
{
    m_serverSettingAction = new QAction(tr("remote server config"), this);///远程服务配置
    connect(m_serverSettingAction, SIGNAL(triggered()), this, SLOT(OnServerActionTrigger()));

    m_localSettingAction = new QAction(tr("local server config"), this);///本地服务配置
    connect(m_localSettingAction, SIGNAL(triggered()), this, SLOT(OnLocalActionTrigger()));
}

void UkeySystemTrayIcon::CreateTrayIcon()
{
    m_trayIconMenu = new QMenu(this);
    m_trayIconMenu->addAction(m_serverSettingAction);
    m_trayIconMenu->addAction(m_localSettingAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayIconMenu);
    m_trayIcon->setIcon(QIcon(":/images/tray_noAuth.png"));
    m_trayIcon->setToolTip(tr("Identity authentication system client"));
}
