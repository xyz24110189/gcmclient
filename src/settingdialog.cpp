#include "settingdialog.h"
#include "httpserverrunable.h"
#include "httpclientagent.h"
#include "confighandle.h"
#include <QtCore/QString>
#include <QtCore/QRegExp>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRegExpValidator>


ServerSettingDialog::ServerSettingDialog(QDialog *parent)
    : QDialog(parent)
{
    InitProperty();
    InitControls();
    InitConnections();
}

ServerSettingDialog::~ServerSettingDialog()
{

}

void ServerSettingDialog::Show()
{
	std::string strAddr;
	unsigned short port = 0;
	ConfigHandle handle;
	handle.InitConfig();
	handle.GetRemoteServerConf(strAddr, port);
	m_hostEdit->setText(strAddr.data());
	m_portEdit->setText(QString::number(port));
	this->exec();
}

void ServerSettingDialog::OnConfirmReleased()
{
    std::string strAddr = m_hostEdit->text().toLocal8Bit().data();
    unsigned short port = m_portEdit->text().toUInt();
	ConfigHandle handle;
	handle.InitConfig();
	handle.SetRemoteServerConf(strAddr, port);
    httpClientAgent::Instance()->ReInitClient();
    this->close();
}

void ServerSettingDialog::InitProperty()
{
	this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint);
    this->setWindowTitle(tr("remote server config"));///远程服务配置
	this->setWindowIcon(QIcon(":/images/tray.png"));
	this->setFixedSize(400, 200);
}

void ServerSettingDialog::InitControls()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *centerLayout = new QGridLayout(this);
    QHBoxLayout *bottomLayout = new QHBoxLayout(this);
    m_hostEdit = new QLineEdit(this);
    m_portEdit = new QLineEdit(this);
    m_confirmButton = new QPushButton(tr("confirm"), this);///确定

    QRegExp ipRx("^((25[0-5]|2[0-4]\\d|[01]?\\d\\d?)\\.){3}(25[0-5]|2[0-4]\\d|[01]?\\d\\d?)$");
    QRegExp portRx("^([0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
    QValidator *ipValidator = new QRegExpValidator(ipRx, this);
    QValidator *portValidator = new QRegExpValidator(portRx, this);
    m_hostEdit->setValidator(ipValidator);
    m_portEdit->setValidator(portValidator);
	m_hostEdit->setFixedSize(200, 27);
	m_portEdit->setFixedSize(200, 27);
	m_confirmButton->setFixedSize(100, 27);
	centerLayout->setContentsMargins(50, 0, 50, 0);
	bottomLayout->setAlignment(Qt::AlignRight);

    centerLayout->addWidget(new QLabel(tr("server addr:"), this), 0, 0);///服务地址
    centerLayout->addWidget(m_hostEdit, 0, 1);
    centerLayout->addWidget(new QLabel(tr("server port:"), this), 1, 0);///服务端口
    centerLayout->addWidget(m_portEdit, 1, 1);

    bottomLayout->addWidget(m_confirmButton);
	bottomLayout->addSpacing(50);

    mainLayout->addLayout(centerLayout);
    mainLayout->addLayout(bottomLayout);
    this->setLayout(mainLayout);
}

void ServerSettingDialog::InitConnections()
{
    connect(m_confirmButton, SIGNAL(released()), this, SLOT(OnConfirmReleased()));
}


LocalSettingDialog::LocalSettingDialog(QDialog *parent)
    : QDialog(parent)
{
    InitProperty();
    InitControls();
    InitConnections();
}

LocalSettingDialog::~LocalSettingDialog()
{

}

void LocalSettingDialog::Show()
{
	unsigned short port = 0;
	ConfigHandle handle;
	handle.InitConfig();
	handle.GetLocalServerPort(port);
	m_portEdit->setText(QString::number(port));
	this->exec();
}

void LocalSettingDialog::OnConfirmReleased()
{
	unsigned short port = m_portEdit->text().toUInt();
	ConfigHandle handle;
	handle.InitConfig();
	handle.SetLocalServerPort(port);
	HttpServerRunable::Instance()->DestroyServer();
    this->close();
}

void LocalSettingDialog::InitProperty()
{
	this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint);
    this->setWindowTitle(tr("local server config"));///本地服务配置
	this->setWindowIcon(QIcon(":/images/tray.png"));
	this->setFixedSize(400, 200);
}

void LocalSettingDialog::InitControls()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *centerLayout = new QHBoxLayout(this);
    QHBoxLayout *bottomLayout = new QHBoxLayout(this);
    m_portEdit = new QLineEdit(this);
    m_confirmButton = new QPushButton(tr("confirm"), this);///确定

    QRegExp portRx("^([0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
    QValidator *portValidator = new QRegExpValidator(portRx, this);
    m_portEdit->setValidator(portValidator);
	m_portEdit->setFixedSize(200, 27);
	m_confirmButton->setFixedSize(100, 27);
	centerLayout->setContentsMargins(50, 0, 50, 0);
	bottomLayout->setAlignment(Qt::AlignRight);

    centerLayout->addWidget(new QLabel(tr("server port:"), this));///服务端口
    centerLayout->addWidget(m_portEdit);

    bottomLayout->addWidget(m_confirmButton);
	bottomLayout->addSpacing(50);

    mainLayout->addLayout(centerLayout);
    mainLayout->addLayout(bottomLayout);
    this->setLayout(mainLayout);
}

void LocalSettingDialog::InitConnections()
{
    connect(m_confirmButton, SIGNAL(released()), this, SLOT(OnConfirmReleased()));
}


