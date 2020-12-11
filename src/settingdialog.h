#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class ServerSettingDialog : public QDialog
{
	Q_OBJECT
public:
    ServerSettingDialog(QDialog *parent = NULL);
    ~ServerSettingDialog();

	void Show();

protected slots:
    void OnConfirmReleased();

protected:
    void InitProperty();
    void InitControls();
    void InitConnections();

private:
    QLineEdit   *m_hostEdit;
    QLineEdit   *m_portEdit;
    QPushButton *m_confirmButton;
};

class LocalSettingDialog : public QDialog
{
	Q_OBJECT
public:
    LocalSettingDialog(QDialog *parent = NULL);
    ~LocalSettingDialog();

	void Show();

protected slots:
    void OnConfirmReleased();

protected:
    void InitProperty();
    void InitControls();
    void InitConnections();

private:
    QLineEdit   *m_portEdit;
    QPushButton *m_confirmButton;
};

#endif // SETTINGDIALOG_H
