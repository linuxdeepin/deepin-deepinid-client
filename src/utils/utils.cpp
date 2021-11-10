
#include "utils.h"

#include <QLocale>
#include <QDBusInterface>
#include <QDebug>
#include <QRegExp>
#include <QProcess>
#include <dsysinfo.h>
#include <QDBusReply>

#include <DGuiApplicationHelper>

#include <com_deepin_deepinid.h>

DCORE_USE_NAMESPACE

namespace utils
{

QString authCodeURL(const QString &clientID,
                    const QStringList &scopes,
                    const QString &callback,
                    const QString &state)
{
    QString templateURL = "%1/oauth2/authorize";
    templateURL += "?response_type=code";
    templateURL += "&client_id=%3";
    templateURL += "&redirect_uri=%4";
    templateURL += "&scope=%5";
    templateURL += "&state=%6";
    templateURL += "&lang=%7";
    templateURL += "&theme=%8";
    templateURL += "&color=%9";
    templateURL += "&font_family=%10";
    templateURL += "&display=sync";
    templateURL += "&version=2.0";
    templateURL += "&handle_open_link=true";
    templateURL += "&client_version=%11";
    templateURL += "&device_kernel=%12";
    templateURL += "&device_processor=%13";
    templateURL += "&os_version=%14";
    templateURL += "&device_code=%15";
    templateURL += "&user_name=%16";
    templateURL += "&device_name=%17";

    qDebug() << Q_FUNC_INFO << __LINE__ << qApp->applicationVersion();

    QStringList deviceInfo = getDeviceInfo();
    QString oauthURI = "https://login.uniontech.com";

    if (!qEnvironmentVariableIsEmpty("DEEPINID_OAUTH_URI")) {
        oauthURI = qgetenv("DEEPINID_OAUTH_URI");
    }

    auto url = QString(templateURL).
        arg(oauthURI).
        arg(clientID).
        arg(callback).
        arg(scopes.join(",")).
        arg(state).
        arg(QLocale().name()).
        arg(getThemeName()).
        arg(getActiveColor()).
        arg(getStandardFont()).
        arg(qApp->applicationVersion()).
        arg(getDeviceKernel()).
        arg(getDeviceProcessor()).
        arg(getOsVersion()).
        arg(getDeviceCode()).
        arg(deviceInfo.at(0)).
        arg(deviceInfo.at(1));
    return url.remove(QRegExp("#"));
}

QString authCodeURL(const QString &path,
                    const QString &clientID,
                    const QStringList &scopes,
                    const QString &callback,
                    const QString &state)
{
    QString templateURL = "%1%2";
    templateURL += "?response_type=code";
    templateURL += "&client_id=%3";
    templateURL += "&redirect_uri=%4";
    templateURL += "&scope=%5";
    templateURL += "&state=%6";
    templateURL += "&lang=%7";
    templateURL += "&theme=%8";
    templateURL += "&color=%9";
    templateURL += "&font_family=%10";
    templateURL += "&display=sync";
    templateURL += "&version=2.0";
    templateURL += "&handle_open_link=true";
    templateURL += "&client_version=%11";
    templateURL += "&device_kernel=%12";
    templateURL += "&device_processor=%13";
    templateURL += "&os_version=%14";
    templateURL += "&device_code=%15";
    templateURL += "&user_name=%16";
    templateURL += "&device_name=%17";

    QString oauthURI = "https://login.uniontech.com";
    QStringList deviceInfo = getDeviceInfo();

    qDebug() << Q_FUNC_INFO << __LINE__ << qApp->applicationVersion();

    if (!qEnvironmentVariableIsEmpty("DEEPINID_OAUTH_URI")) {
        oauthURI = qgetenv("DEEPINID_OAUTH_URI");
    }

    auto url = QString(templateURL).
        arg(oauthURI).
        arg(path).
        arg(clientID).
        arg(callback).
        arg(scopes.join(",")).
        arg(state).
        arg(QLocale().name()).
        arg(getThemeName()).
        arg(getActiveColor()).
        arg(getStandardFont()).
        arg(qApp->applicationVersion()).
        arg(getDeviceKernel()).
        arg(getDeviceProcessor()).
        arg(getOsVersion()).
        arg(getDeviceCode()).
        arg(deviceInfo.at(0)).
        arg(deviceInfo.at(1));

    return url.remove(QRegExp("#"));
}

QString getThemeName()
{
    auto themeType = Dtk::Gui::DGuiApplicationHelper::instance()->themeType();

    switch (themeType) {
    case Dtk::Gui::DGuiApplicationHelper::DarkType:
        return "dark";
    case Dtk::Gui::DGuiApplicationHelper::LightType:
    case Dtk::Gui::DGuiApplicationHelper::UnknownType:
    default:
        return "light";
    }
}

QString getActiveColor()
{
    QDBusInterface appearance_ifc_(
                "com.deepin.daemon.Appearance",
                "/com/deepin/daemon/Appearance",
                "com.deepin.daemon.Appearance",
                QDBusConnection::sessionBus()
                );
    qDebug() << "connect" << "com.deepin.daemon.Appearance" << appearance_ifc_.isValid();
    return appearance_ifc_.property("QtActiveColor").toString();
}

QString getStandardFont(){
    QDBusInterface appearance_ifc_(
                "com.deepin.daemon.Appearance",
                "/com/deepin/daemon/Appearance",
                "com.deepin.daemon.Appearance",
                QDBusConnection::sessionBus()
                );
    qDebug() << "connect" << "com.deepin.daemon.Appearance" << appearance_ifc_.isValid();
    return appearance_ifc_.property("StandardFont").toString();
}

void sendDBusNotify(const QString &message)
{
    QStringList actions = QStringList() << "_open" << QObject::tr("View");
    QVariantMap hints;
    hints["x-deepin-action-_open"] = "dde-control-center,-m,cloudsync";

    QList<QVariant> argumentList;
    argumentList << "deepin-deepinid-client";
    argumentList << static_cast<uint>(0);
    argumentList << "com.deepin.deepinid.Client";
    argumentList << "";
    argumentList << message;
    argumentList << actions;
    argumentList << hints;
    argumentList << static_cast<int>(5000);

    static QDBusInterface notifyApp("org.freedesktop.Notifications",
                                    "/org/freedesktop/Notifications",
                                    "org.freedesktop.Notifications");
    notifyApp.callWithArgumentList(QDBus::Block, "Notify", argumentList);
}

// QLocale::system().name(): zh_CN/en_US
// zh/en is lang
QString getLang(const QString &region)
{
    if (region == "CN") {
        return "zh_CN";
    }

    auto locale = QLocale::system().name();
    if (locale.startsWith("zh_")) {
        return "zh_CN";
    }
    return "en_US";
}

bool isTablet(){
    //DGuiApplicationHelper::instance()->isTabletEnvironment();
    return false;
}

QString getDeviceType()
{
    if(isTablet()){
        return "display=tablet";    //识别为平板模式
    }else {
        return "display=sync";      //识别为PC模式
    }
}

QString getDeviceKernel()
{
    QProcess process;
    process.start("uname -r");
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput();
    int idx = output.indexOf('\n');
    if ( -1 != idx) {
        output.remove(idx, 1);
    }
    return output.data();
}

QString getOsVersion()
{
    QString version;
    if (DSysInfo::uosType() == DSysInfo::UosServer || DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
        version = QString("%1%2").arg(DSysInfo::minorVersion())
                                  .arg(DSysInfo::uosEditionName());
    } else if (DSysInfo::isDeepin()) {
        version = QString("%1 (%2)").arg(DSysInfo::uosEditionName())
                                  .arg(DSysInfo::minorVersion());
    } else {
        version = QString("%1 %2").arg(DSysInfo::productVersion())
                                  .arg(DSysInfo::productTypeString());
    }
    return version;
}

QString getDeviceProcessor()
{
    QDBusInterface Interface("com.deepin.daemon.SystemInfo",
                             "/com/deepin/daemon/SystemInfo",
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::sessionBus());
    QDBusMessage reply = Interface.call("Get", "com.deepin.daemon.SystemInfo", "CPUMaxMHz");
    QList<QVariant> outArgs = reply.arguments();
    double cpuMaxMhz = outArgs.at(0).value<QDBusVariant>().variant().toDouble();
    if (DSysInfo::cpuModelName().contains("Hz")) {
        return DSysInfo::cpuModelName();
    } else {
        if (DSysInfo::cpuModelName().isEmpty()){
            QDBusMessage replyCpuInfo = Interface.call("Get", "com.deepin.daemon.SystemInfo", "Processor");
            QList<QVariant> outArgsCpuInfo = replyCpuInfo.arguments();
            QString processor = outArgsCpuInfo.at(0).value<QDBusVariant>().variant().toString();
            return QString("%1 @ %2GHz").arg(processor)
                                  .arg(cpuMaxMhz / 1000);
        } else {
            return QString("%1 @ %2GHz").arg(DSysInfo::cpuModelName())
                                  .arg(cpuMaxMhz / 1000);
        }
    }
}

QString getDeviceCode()
{
    QDBusInterface Interface("com.deepin.deepinid",
                             "/com/deepin/deepinid",
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::sessionBus());
    QDBusMessage reply = Interface.call("Get", "com.deepin.deepinid", "HardwareID");
    QList<QVariant> outArgs = reply.arguments();
    QString deviceCode = outArgs.at(0).value<QDBusVariant>().variant().toString();
    return deviceCode;
}

QStringList getDeviceInfo()
{
    qDBusRegisterMetaType<HardwareInfo>();
    QDBusInterface licenseInfo("com.deepin.sync.Helper",
                               "/com/deepin/sync/Helper",
                               "com.deepin.sync.Helper",
                               QDBusConnection::systemBus());

    QDBusReply<HardwareInfo> hardwareInfo = licenseInfo.call(QDBus::AutoDetect, "GetHardware");
    QJsonObject jsonObject;
    auto hardwareInfoValue = hardwareInfo.value();
    auto hardwareDMIValue = hardwareInfo.value().dmi;

    qDebug() << hardwareInfo.error();
    // -pc
    QString userName = hardwareInfoValue.hostName;
    QString Vendor = hardwareDMIValue.boardVendor;

    QStringList deviceInfo;
    deviceInfo.append(userName);
    deviceInfo.append(Vendor);
    return deviceInfo;
}

};
