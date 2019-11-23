#include <QIcon>
#include <QDBusError>
#include <QDBusConnection>

#include <DApplication>
#include <DLog>
#include <DPlatformWindowHandle>
#include <DStandardPaths>
#include <DGuiApplicationHelper>

#include "ui/login_window.h"

namespace Const
{

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

    Dtk::Widget::DApplication::setOrganizationName("deepin");

    if (!DGuiApplicationHelper::setSingleInstance("com.deepin.deepinid.Client")) {
        qWarning() << "another client is running";
        return 0;
    }

    if (!Dtk::Widget::DPlatformWindowHandle::pluginVersion().isEmpty()) {
        Dtk::Widget::DApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
    }

    QCommandLineParser parser;
    QCommandLineOption bootstrap({"b", "bootstrap"}, "start up url", "url", "");
    QCommandLineOption daemon({"d", "daemon"}, "run in background");
    parser.addOption(bootstrap);
    parser.addOption(daemon);
    parser.addHelpOption();

    parser.process(app);

    app.loadTranslator();

    ddc::LoginWindow lw;

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

    if (parser.isSet(daemon)) {
        Dtk::Widget::DApplication::setQuitOnLastWindowClosed(false);
    }

    auto iconPath = ":/web/com.deepin.deepinid.Client.svg";
    Dtk::Widget::DApplication::setWindowIcon(QIcon(iconPath));
    lw.setWindowIcon(QIcon(iconPath));

    return Dtk::Widget::DApplication::exec();
}
