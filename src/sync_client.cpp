#include "sync_client.h"

#include <QDebug>
#include <QtDBus>
#include <QVariantMap>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QProcess>

#include <QUrl>
#include <QDesktopServices>
#include <QCoreApplication>

#include "deepinid_interface.h"

namespace Const
{
const auto SyncDaemonService   = "com.deepin.deepinid";
const auto SyncDaemonPath      = "/com/deepin/deepinid";
//const auto SyncDaemonInterface = "com.deepin.deepinid";
}

namespace dsc
{


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
                                    "/org/freedesktop/Notifications", "org.freedesktop.Notifications");
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

QString getPrivacyPolicyPath(const QString &region)
{
    auto prefix = "/usr/share/deepin-deepinid-client/privacy";
    return QString("%1/deepinid-%2-%3.txt").arg(prefix).arg(region).arg(getLang(region));
}

bool confirmPrivacyPolicy(QString region)
{
    if (region.isEmpty()) {
        region = "CN";
    }

    QProcess ddeLicenseDialog;
    QString title = QObject::tr("Deepin ID Privacy Policy");
    QString allowHint = QObject::tr("Agree and Turn On Cloud Sync");
    QString privacyPolicyPath = getPrivacyPolicyPath(region);
    ddeLicenseDialog.setProgram("dde-license-dialog");
    QStringList args;
    args << "-t" << title
         << "-c" << privacyPolicyPath
         << "-a" << allowHint;

    ddeLicenseDialog.setArguments(args);
    ddeLicenseDialog.start();

    if (!ddeLicenseDialog.waitForStarted(-1)) {
        qWarning() << "start dde-license-dialog failed" << ddeLicenseDialog.state();
        sendDBusNotify(SyncClient::tr("Login failed"));
        return false;
    }

    ddeLicenseDialog.waitForFinished(-1);

    return ddeLicenseDialog.exitCode() == 96;
}

class SyncClientPrivate
{
public:
    SyncClientPrivate(SyncClient *parent) : q_ptr(parent)
    {

        daemonIf = new DeepinIDInterface(Const::SyncDaemonService,
                                         Const::SyncDaemonPath,
                                         QDBusConnection::sessionBus());
    }

    DeepinIDInterface *daemonIf;

    SyncClient *q_ptr;
    Q_DECLARE_PUBLIC(SyncClient)
};

SyncClient::SyncClient(QObject *parent) :
    QObject(parent), dd_ptr(new SyncClientPrivate(this))
{

}

SyncClient::~SyncClient()
{

}

QString SyncClient::machineID() const
{
    Q_D(const SyncClient);
    return d->daemonIf->property("HardwareID").toString();
}

bool SyncClient::logined() const
{
    Q_D(const SyncClient);

    auto userInfo = d->daemonIf->userInfo();
    return userInfo.value("IsLoggedIn").toBool();
}

QString SyncClient::gettext(const QString &str)
{
    return tr(str.toStdString().c_str());
}

void SyncClient::setToken(const QVariantMap &tokenInfo)
{
    Q_D(SyncClient);

    Q_EMIT this->requestHide();

    if (confirmPrivacyPolicy(tokenInfo.value("region").toString())) {
        auto reply = d->daemonIf->SetToken(tokenInfo);
        qDebug() << "set token with reply:" << reply.error();
        if (reply.error().isValid()) {
            sendDBusNotify(tr("Login failed"));
        } else {
            sendDBusNotify(tr("Login successful, please go to Cloud Sync to view the settings"));
        }
    }

    qApp->quit();
}

void SyncClient::open(const QString &url)
{
    QDesktopServices::openUrl(url);
}

void SyncClient::close()
{
    Q_EMIT this->prepareClose();
    qApp->quit();
}

}
