#include <QIcon>
#include <QDBusError>
#include <QDBusConnection>

#include <DApplication>
#include <DLog>
#include <DPlatformWindowHandle>
#include <DStandardPaths>

#include <qcef_context.h>
#include <qcef_web_settings.h>

#include "login_window.h"

namespace Const
{
const char EnableDomStorageFlush[] = "--enable-aggressive-domstorage-flushing";
const char DisableGpu[] = "--disable-gpu";
const char EnableLogging[] = "--enable-logging";
const char LogLevel[] = "--log-level";

const char DBusService[] = "com.deepin.deepinid.Client";
const char DBusPath[] = "/com/deepin/deepinid/Client";
} // namespace Const

bool initQCef(int argc, char **argv)
{
    QCefGlobalSettings settings;
    // Do not use sandbox.
    settings.setNoSandbox(true);

    if (qEnvironmentVariableIntValue("QCEF_DEBUG") == 1) {
        // Open http://localhost:9222 in chromium browser to see dev tools.
        settings.setRemoteDebug(true);
        settings.setLogSeverity(QCefGlobalSettings::LogSeverity::Verbose);
    } else {
        settings.setRemoteDebug(false);
        settings.setLogSeverity(QCefGlobalSettings::LogSeverity::Error);
    }

    settings.setIgnoresCertificateErrors(true);

    // Disable GPU process.
    settings.addCommandLineSwitch(Const::DisableGpu, "");

    // Enable aggressive storage commit to minimize data loss.
    // See public/common/content_switches.cc.
    settings.addCommandLineSwitch(Const::EnableDomStorageFlush, "");

    QDir cacheDir(Dtk::Core::DStandardPaths::standardLocations(QStandardPaths::CacheLocation).value(0));
    QDir appCacheDir(cacheDir.absoluteFilePath("deepin/deepin-sync-client"));
    appCacheDir.mkpath(".");

    settings.setCachePath(appCacheDir.filePath("cache"));
    settings.setUserDataPath(appCacheDir.filePath("cef-storage"));

    settings.setLogFile(appCacheDir.filePath("web-console.log"));
    settings.addCommandLineSwitch(Const::EnableLogging, "");
    settings.addCommandLineSwitch(Const::LogLevel, "0");

    settings.setBackgroundColor(0x00FFFFFF);
    auto ret = QCefInit(argc, argv, settings);
    if (ret >= 0) {
        qCritical() << "init qcef failed" << ret;
        return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    qputenv("DXCB_FAKE_PLATFORM_NAME_XCB", "true");
    qputenv("DXCB_REDIRECT_CONTENT", "true");

    if (!initQCef(argc, argv)) {
        return -1;
    }

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    Dtk::Widget::DApplication::loadDXcbPlugin();
    Dtk::Widget::DApplication app(argc, argv);

    if (!app.setSingleInstance("com.deepin.deepinid.Client")) {
        qWarning() << "another client is running";
        return 0;
    }

    if (!Dtk::Widget::DPlatformWindowHandle::pluginVersion().isEmpty()) {
        app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
    }
    QCefBindApp(&app);

    QCommandLineParser parser;
    QCommandLineOption bootstrap({"b", "bootstrap"}, "start up url", "url", "");
    QCommandLineOption daemon({"d", "daemon"}, "run in background");
    parser.addOption(bootstrap);
    parser.addOption(daemon);
    parser.addHelpOption();

    parser.process(app);

    app.loadTranslator();

    dsc::LoginWindow lw;

    if (lw.logined()) {
        qWarning() << "user has logined";
        return 0;
    }
    lw.setFixedSize(360, 430);

    auto sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.registerService(Const::DBusService)) {
        qDebug() << "register service failed" << sessionBus.lastError();
        return -1;
    }
    if (!sessionBus.registerObject(Const::DBusPath,
                                   &lw,
                                   QDBusConnection::ExportScriptableSlots)) {
        qDebug() << "register object failed" << sessionBus.lastError();
        return -2;
    }

    if (parser.isSet(bootstrap)) {
        lw.setURL(parser.value(bootstrap));
    }

    if (!parser.isSet(daemon)) {
        lw.Show();
    }
    app.setWindowIcon(QIcon(":/web/cloud_offline.svg"));
    lw.setWindowIcon(QIcon(":/web/cloud_offline.svg"));
    return app.exec();
}
