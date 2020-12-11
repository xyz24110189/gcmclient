#include "ukeyverifywidget.h"
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QtCore/QString>
#include <QtCore/QRegExp>
#include <QtCore/QDebug>

UkeyVerifyWidget::UkeyVerifyWidget(QDialog *parent)
    : QDialog(parent), m_verifyFlag(false)
{
    InitProperty();
    InitControls();
    InitConnection();
}

UkeyVerifyWidget::~UkeyVerifyWidget()
{

}

void UkeyVerifyWidget::SetVerify(bool isVerify)
{
    m_verifyFlag = isVerify;
    if (m_verifyFlag)
    {
        m_pinEdit->clear();
        this->hide();
    }
}

void UkeyVerifyWidget::SetAppList(std::vector<std::string> &appList)
{
	m_appsComboBox->clear();
	for (int i = 0; i < appList.size(); ++i)
		m_appsComboBox->addItem(appList[i].data());
	m_appsComboBox->setCurrentIndex(0);
}

void UkeyVerifyWidget::ShowInfomationBox(int errorCode, const VerifyPinInfo &info)
{
	if (errorCode == 0) return;

	QMessageBox msgBox(this);
	QString msg = tr("password is not correct, you still have") + info.pulRetryCount.data() + tr("chances");
	msgBox.setWindowTitle(tr("prompt"));
	msgBox.setWindowIcon(QIcon(":/images/tray.png"));
	msgBox.setText(msg);
	msgBox.exec();
}

void UkeyVerifyWidget::InitProperty()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint |
                         Qt::Tool |Qt::WindowStaysOnTopHint);
    this->setWindowTitle(tr("PIN verify control"));///PIN码验证框
	this->setWindowIcon(QIcon(":/images/tray.png"));
    this->setFixedSize(400, 200);
}

void UkeyVerifyWidget::InitControls()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *hboxLayoutTop = new QHBoxLayout(this);
	QHBoxLayout *hboxLayoutCenter = new QHBoxLayout(this);
    QHBoxLayout *hboxLayoutBottom = new QHBoxLayout(this);
    QLabel *appsLabel = new QLabel(tr("app list:"), this);///应用列表
    QLabel *pinLabel = new QLabel(tr("input PIN:"), this);///输入PIN码
    m_pinEdit = new QLineEdit(this);
	m_appsComboBox = new QComboBox(this);
    m_confirmButton = new QPushButton(tr("confirm"), this);///确定
	QRegExp rx("^[^\u4E00-\u9FA5]+$");///^\\w+$
    QValidator *validator = new QRegExpValidator(rx, this);

    mainLayout->setSpacing(10);
	hboxLayoutCenter->setAlignment(Qt::AlignCenter);
    hboxLayoutBottom->setAlignment(Qt::AlignRight);
    m_pinEdit->setFixedSize(250, 27);
    m_pinEdit->setValidator(validator);
	m_pinEdit->setEchoMode(QLineEdit::Password);
	m_appsComboBox->setFixedSize(250, 27);
    m_confirmButton->setFixedSize(100, 27);

	hboxLayoutTop->addStretch();
	hboxLayoutTop->addWidget(appsLabel);
	hboxLayoutTop->addWidget(m_appsComboBox);
	hboxLayoutTop->addStretch();

	hboxLayoutCenter->addStretch();
	hboxLayoutCenter->addWidget(pinLabel);
	hboxLayoutCenter->addWidget(m_pinEdit);
	hboxLayoutCenter->addStretch();

    hboxLayoutBottom->addWidget(m_confirmButton);
    hboxLayoutBottom->addSpacing(27);

	mainLayout->addSpacing(20);
	mainLayout->addLayout(hboxLayoutTop);
    mainLayout->addLayout(hboxLayoutCenter);
    mainLayout->addLayout(hboxLayoutBottom);
    mainLayout->addSpacing(10);

    this->setLayout(mainLayout);
}

void UkeyVerifyWidget::InitConnection()
{
    connect(m_confirmButton, SIGNAL(released()), this, SLOT(OnConfirmClicked()));
}

void UkeyVerifyWidget::OnConfirmClicked()
{
	QString strApp = m_appsComboBox->currentText();
    QString strPin = m_pinEdit->text();
    emit VerifyPin(strApp, strPin);
    //qDebug() << strPin;
}
