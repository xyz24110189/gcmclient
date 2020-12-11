#include "wrappkiagent.h"
#include "ukeysystemtray.h"
#include "httpserverrunable.h"
#include "libusbagentrunable.h"
#include "singleapplication.h"
#include <QtGlobal>
#include <QApplication>
#include <QDesktopServices>
#include <QtCore/QThreadPool>
#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtCore/QDir>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QStandardPaths>
#endif

int main(int argc, char *argv[])
{
    SingleApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(false);

	QString logPath;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QTextCodec *utf8Codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(utf8Codec);
	logPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
	QTextCodec *utf8Codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForTr(utf8Codec);
	QTextCodec::setCodecForLocale(utf8Codec);
	QTextCodec::setCodecForCStrings(utf8Codec);

	logPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

	logPath += "/logs";
	QDir dir;
	dir.mkpath(logPath);
	logPath += "/GCMClient.csv";
	plog::init(plog::debug, QDir::toNativeSeparators(logPath).toLocal8Bit().data(), M_SIZE * 10, 5/*log file size*/);

	QTranslator translator;
	QString appPath = a.applicationDirPath();
	appPath += QDir::separator();
	appPath += TRANSLATION_FILE;
	translator.load(appPath);
	a.installTranslator(&translator);

	QThreadPool::globalInstance()->start(HttpServerRunable::Instance());

	UkeySystemTrayIcon trayIcon;
	trayIcon.Show();

    WrapPkiAgent *inst = WrapPkiAgent::GetInstance();
    inst->InitAgent();

	QObject::connect(inst, SIGNAL(AuthChange(bool)),
		&trayIcon, SLOT(OnAuthChange(bool)), Qt::AutoConnection);

#ifndef _WIN32
    //LibUsbAgentRunable usbAgent;
    //QThreadPool::globalInstance()->start(&usbAgent);
#endif

    return a.exec();
}
