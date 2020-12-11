#ifndef UKEYVERIFYWIDGET_H
#define UKEYVERIFYWIDGET_H

#include "commontype.h"
#include <QDialog>
#include <QtCore/QString>

class QLineEdit;
class QPushButton;
class QComboBox;

class UkeyVerifyWidget : public QDialog
{
    Q_OBJECT

public:
    UkeyVerifyWidget(QDialog *parent = 0);
    ~UkeyVerifyWidget();

    void SetVerify(bool isVerify = true);
    bool Verify() { return m_verifyFlag; }

	void SetAppList(std::vector<std::string> &appList);

	void ShowInfomationBox(int errorCode, const VerifyPinInfo &info);

signals:
    void VerifyPin(const QString &app, const QString &pin);

protected slots:
    void OnConfirmClicked();

protected:
    void InitProperty();
    void InitControls();
    void InitConnection();

private:
    bool        m_verifyFlag;
    QLineEdit   *m_pinEdit;
	QComboBox   *m_appsComboBox;
    QPushButton *m_confirmButton;
};

#endif // UKEYVERIFYWIDGET_H
