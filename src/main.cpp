#include <QIcon>
#include <QDBusError>
#include <QDBusConnection>

#include <DApplication>
#include <DLog>
#include <DPlatformWindowHandle>
#include <DStandardPaths>

#include "login_window.h"

namespace Const
{
//const char EnableDomStorageFlush[] = "--enable-aggressive-domstorage-flushing";
//const char DisableGpu[] = "--disable-gpu";
//const char EnableLogging[] = "--enable-logging";
//const char LogLevel[] = "--log-level";

const char DBusService[] = "com.deepin.deepinid.Client";
const char DBusPath[] = "/com/deepin/deepinid/Client";
} // namespace Const

int main(int argc, char **argv)
{
    qputenv("DXCB_FAKE_PLATFORM_NAME_XCB", "true");
    qputenv("DXCB_REDIRECT_CONTENT", "true");

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

    auto iconPath = ":/web/com.deepin.deepinid.Client.svg";
    app.setWindowIcon(QIcon(iconPath));
    lw.setWindowIcon(QIcon(iconPath));
    return app.exec();
}
