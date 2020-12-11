#ifndef UKEYSYSTEMTRAY_H
#define UKEYSYSTEMTRAY_H

#include <QWidget>

class QMenu;
class QAction;
class QSystemTrayIcon;

class UkeySystemTrayIcon : public QWidget
{
    Q_OBJECT
public:
    UkeySystemTrayIcon();
    ~UkeySystemTrayIcon();

	void Show();

public slots:
	void OnAuthChange(bool auth);

protected slots:
    void OnLocalActionTrigger();
    void OnServerActionTrigger();

protected:
    void CreateActions();
    void CreateTrayIcon();

private:
    QMenu *m_menu;
    QAction *m_action;
    QMenu *m_trayIconMenu;
    QAction *m_serverSettingAction;
    QAction *m_localSettingAction;
    QSystemTrayIcon *m_trayIcon;
};

#endif // UKEYSYSTEMTRAY_H
