#include <QIcon>
#include <QDBusError>
#include <QDBusConnection>
#include <QSurfaceFormat>
#include <QProcess>

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
#ifdef __sw_64__
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--no-sandbox");
#endif

    //Disable function: Qt::AA_ForceRasterWidgets, solve the display problem of domestic platform (loongson mips)
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu");
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-web-security");
    qputenv("DXCB_FAKE_PLATFORM_NAME_XCB", "true");

    // 系统使用自动代理时，程序会闪退，不知道原因暂时规避
    const QString auto_proxy = qgetenv("auto_proxy");
    if (!auto_proxy.isEmpty()) {
        qputenv("auto_proxy", "");
    }

    //龙芯机器配置,使得DApplication能正确加载QTWEBENGINE
    qputenv("DTK_FORCE_RASTER_WIDGETS", "FALSE");

    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
    qputenv("_d_disableDBusFileDialog", "true");
    setenv("PULSE_PROP_media.role", "video", 1);

    // 读取系统
    QProcess process;
    process.start("lspci -d 1ed5:");
    process.waitForFinished(500);
    bool isMoore = !QString::fromLocal8Bit(process.readAllStandardOutput()).isEmpty();

    QSurfaceFormat format;
    if (isMoore) {
        format.setRenderableType(QSurfaceFormat::OpenGL);
    } else {
        format.setRenderableType(QSurfaceFormat::DefaultRenderableType);
    }
#ifdef __sw_64__
    format.setRenderableType(QSurfaceFormat::OpenGL);
#endif

#ifdef __mips64
    format.setRenderableType(QSurfaceFormat::OpenGL);
#endif

    QString glstring = QString::fromLocal8Bit(qgetenv("DEBUG_OPENGL"));
    if(!glstring.isEmpty())
    {
        int gltype = glstring.toInt();
        qDebug() << "set gltype:" << gltype;
        if(gltype == 1)
        {
            format.setRenderableType(QSurfaceFormat::DefaultRenderableType);
        }
        else if (gltype == 2)
        {
            format.setRenderableType(QSurfaceFormat::OpenGL);
        }
        else if (gltype == 3)
        {
            format.setRenderableType(QSurfaceFormat::OpenGLES);
        }
    }

    qDebug() << "set surface format:" << format.renderableType();
    QSurfaceFormat::setDefaultFormat(format);

    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--single-process");

    Dtk::Widget::DApplication app(argc, argv);
    app.setAttribute(Qt::AA_ForceRasterWidgets, false);
    app.setOrganizationName("deepin");
    app.setApplicationDisplayName(QObject::tr("UOS ID"));
    app.setProductIcon(QIcon::fromTheme("uos_id"));
    app.setWindowIcon(QIcon::fromTheme("uos_id"));

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

#ifdef CVERSION
    QString verstr(CVERSION);
    if (verstr.isEmpty() || verstr.contains("1.0.0"))
        verstr = "5.3.1.1";
    app.setApplicationVersion(verstr);
#else
    app.setApplicationVersion("5.3.1.2");
#endif

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
    lw.setWindowIcon(QIcon::fromTheme("uos_id"));

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

    Dtk::Widget::DApplication::setQuitOnLastWindowClosed(true);

    if (parser.isSet(bootstrap)) {
        lw.setURL(parser.value(bootstrap));
        lw.load();
        lw.show();
    }

    QLoggingCategory::setFilterRules(QLatin1String("*.*.debug=false"));

    if (parser.isSet(verbose)) {
        QLoggingCategory::setFilterRules(QLatin1String("ui.sync_client.debug=true"));
    }

    return Dtk::Widget::DApplication::exec();
}
