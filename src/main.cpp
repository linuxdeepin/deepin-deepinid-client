#include <QIcon>
#include <QDBusError>
#include <QDBusConnection>
#include <QSurfaceFormat>

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
    Dtk::Widget::DApplication::loadDXcbPlugin();
    //Disable function: Qt::AA_ForceRasterWidgets, solve the display problem of domestic platform (loongson mips)
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu");
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-web-security");
    qputenv("DXCB_FAKE_PLATFORM_NAME_XCB", "true");

    //龙芯机器配置,使得DApplication能正确加载QTWEBENGINE
    qputenv("DTK_FORCE_RASTER_WIDGETS", "FALSE");

    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
    qputenv("_d_disableDBusFileDialog", "true");
    setenv("PULSE_PROP_media.role", "video", 1);
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGLES);
    format.setDefaultFormat(format);

    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--single-process");

    Dtk::Widget::DApplication app(argc, argv);

    app.setAttribute(Qt::AA_ForceRasterWidgets, false);
    app.setOrganizationName("deepin");
    app.setApplicationDisplayName(QObject::tr("Union ID"));

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    if (!DGuiApplicationHelper::setSingleInstance("com.deepin.deepinid.Client")) {
        qWarning() << "another client is running";
        return 0;
    }

    QCommandLineParser parser;
    QCommandLineOption bootstrap({"b", "bootstrap"}, "start up url", "url", "");
    QCommandLineOption daemon({"d", "daemon"}, "run in background");
    QCommandLineOption verbose("verbose", "debug switch");
    parser.addOption(bootstrap);
    parser.addOption(daemon);
    parser.addOption(verbose);
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

    Dtk::Widget::DApplication::setQuitOnLastWindowClosed(false);

    if (parser.isSet(bootstrap)) {
        lw.setURL(parser.value(bootstrap));
        lw.load();
        lw.show();
    }
    
    QLoggingCategory::setFilterRules(QLatin1String("*.*.debug=false"));

    if (parser.isSet(verbose)) {
        QLoggingCategory::setFilterRules(QLatin1String("ui.sync_client.debug=true"));
    }

    auto iconPath = ":/web/com.deepin.deepinid.Client.svg";
    Dtk::Widget::DApplication::setWindowIcon(QIcon(iconPath));
    lw.setWindowIcon(QIcon(iconPath));

    return Dtk::Widget::DApplication::exec();
}
