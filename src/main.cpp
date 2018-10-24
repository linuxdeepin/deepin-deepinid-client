#include <QIcon>

#include <DLog>
#include <DApplication>
#include <DPlatformWindowHandle>
#include <DStandardPaths>
#include <DWidgetUtil>

#include <qcef_context.h>
#include <qcef_web_settings.h>

#include "login_window.h"

namespace Const
{
const char EnableDomStorageFlush[] = "--enable-aggressive-domstorage-flushing";
const char DisableGpu[] = "--disable-gpu";
const char EnableLogging[] = "--enable-logging";
const char LogLevel[] = "--log-level";
}

int main(int argc, char **argv)
{
    qputenv("DXCB_FAKE_PLATFORM_NAME_XCB", "true");
    qputenv("DXCB_REDIRECT_CONTENT", "true");

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

    QDir cache_dir(Dtk::Core::DStandardPaths::standardLocations(QStandardPaths::CacheLocation).value(0));
    cache_dir.mkpath(".");
    settings.setCachePath(cache_dir.filePath("cache"));
    settings.setUserDataPath(cache_dir.filePath("cef-storage"));

    settings.setLogFile(cache_dir.filePath("web-console.log"));
    settings.addCommandLineSwitch(Const::EnableLogging, "");
    settings.addCommandLineSwitch(Const::LogLevel, "0");

    settings.setBackgroundColor(0x001C1C1C);

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    auto ret = QCefInit(argc, argv, settings);
    if (ret >= 0) {
        qCritical() << "init qcef failed";
        return 0;
    }

    Dtk::Widget::DApplication::loadDXcbPlugin();
    Dtk::Widget::DApplication app(argc, argv);
    if (!Dtk::Widget::DPlatformWindowHandle::pluginVersion().isEmpty()) {
        app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
    }
    QCefBindApp(&app);

    dsc::LoginWindow lw;
    lw.setFixedSize(380, 390);
    lw.show();
    Dtk::Widget::moveToCenter(&lw);

    return app.exec();
}
