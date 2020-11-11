#include "sync_client.h"

#include <QDebug>
#include <QtDBus>
#include <QVariantMap>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QProcess>
#include <QFile>
#include <QUrl>
#include <QDesktopServices>
#include <QCoreApplication>

#include "ipc/const.h"
#include "ipc/deepinid_interface.h"

namespace ddc
{
Q_LOGGING_CATEGORY(deepinid_client, "ui.sync_client")

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

QString getRegionLang(const QString &region, const QString &lang)
{
    return lang;
}

QString getPrivacyPolicyPathByLang(const QString &region, const QString &lang)
{
    const auto defaultRegion = "Other";
    auto prefix = "/usr/share/deepin-deepinid-client/privacy/Privacy-Policy";
    auto privacyPolicyPath = QString("%1/Privacy-Policy-%2-%3.md").
        arg(prefix).
        arg(region).
        arg(getRegionLang(region, lang));

    if (!QFile::exists(privacyPolicyPath) && region != defaultRegion) {
        privacyPolicyPath = getPrivacyPolicyPathByLang(defaultRegion, getRegionLang(region, lang));
    }
    return privacyPolicyPath;
}

class SyncClientPrivate
{
public:
    explicit SyncClientPrivate(SyncClient *parent)
        : q_ptr(parent)
    {

        daemonIf = new DeepinIDInterface(Const::SyncDaemonService,
                                         Const::SyncDaemonPath,
                                         QDBusConnection::sessionBus());
    }

    bool confirmPrivacyPolicy(const QString &id, QString region)
    {
        QDBusReply<bool> reply = daemonIf->HasConfirmPrivacy(id);
        if (!reply.error().isValid() && reply.value()) {
            qWarning() << "HasConfirmPrivacy" << reply.error();
            return true;
        }

        if (region.isEmpty()) {
            region = "CN";
        }

        QString privacyPolicyPathZH = getPrivacyPolicyPathByLang(region, "zh_CN");
        QString privacyPolicyPathEN = getPrivacyPolicyPathByLang(region, "en_US");

        if (!QFile::exists(privacyPolicyPathZH)) {
            qCritical() << "can not find policy text" << privacyPolicyPathZH;
        }

        if (!QFile::exists(privacyPolicyPathEN)) {
            qCritical() << "can not find policy text" << privacyPolicyPathEN;
        }

        QProcess ddeLicenseDialog;
        QString titleZH = "隐私政策";
        QString titleEN = "Privacy Policy";
        QString allowHintZH = "我已阅读并同意《隐私政策》";
        QString allowHintEN = "I have read and agree to the Privacy Policy";
        ddeLicenseDialog.setProgram("dde-license-dialog");
        QStringList args;
        args << "-u" << titleEN
             << "-t" << titleZH
             << "-e" << privacyPolicyPathEN
             << "-c" << privacyPolicyPathZH
             << "-b" << allowHintEN
             << "-a" << allowHintZH;

        ddeLicenseDialog.setArguments(args);
        qDebug() << ddeLicenseDialog.program() << ddeLicenseDialog.arguments().join(" ");

        ddeLicenseDialog.start();

        if (!ddeLicenseDialog.waitForStarted(-1)) {
            qWarning() << "start dde-license-dialog failed" << ddeLicenseDialog.state();
            //sendDBusNotify(SyncClient::tr("Login failed"));
            return false;
        }

        ddeLicenseDialog.waitForFinished(-1);

        auto userConfirm = (ddeLicenseDialog.exitCode() == 96);

        if (userConfirm) {
            daemonIf->ConfirmPrivacy(id);
        }
        return userConfirm;
    }

    DeepinIDInterface *daemonIf;
    QVariantMap session;

    SyncClient *q_ptr;
    Q_DECLARE_PUBLIC(SyncClient)
};

SyncClient::SyncClient(QObject *parent)
    :
    QObject(parent), dd_ptr(new SyncClientPrivate(this))
{

}

SyncClient::~SyncClient() = default;

QString SyncClient::machineID() const
{
    Q_D(
    const SyncClient);
    return d->daemonIf->property("HardwareID").toString();
}

QVariantMap SyncClient::userInfo() const
{
    Q_D(
    const SyncClient);
    return d->daemonIf->property("UserInfo").toMap();
}

QString SyncClient::gettext(const QString &str)
{
    return tr(str.toStdString().c_str());
}

void SyncClient::authCallback(const QVariantMap &tokenInfo)
{
    Q_D(SyncClient);
    qCDebug(deepinid_client) << tokenInfo;
    auto sessionID = tokenInfo.value("session_id").toString();
    auto clientID = tokenInfo.value("client_id").toString();
    auto code = tokenInfo.value("code").toString();
    auto state = tokenInfo.value("state").toString();

    Q_EMIT
    this->requestHide();

    auto region = tokenInfo.value("region").toString();
    auto syncUserID = tokenInfo.value("uid").toString();

    if (code.isEmpty()) {
        //sendDBusNotify(SyncClient::tr("Login failed"));
        Q_EMIT
        this->onCancel(clientID);
        return;
    }

    d->session = tokenInfo;
    if (!callLicenseDialog || d->confirmPrivacyPolicy(syncUserID, region)) {
        //sendDBusNotify(tr("Login successful, please go to Cloud Sync to view the settings"));
        Q_EMIT
        this->onLogin(sessionID, clientID, code, state);
    }
    else {
        //sendDBusNotify(tr("Login failed"));
        Q_EMIT
        this->onCancel(clientID);
    }
}

void SyncClient::open(const QString &url)
{
    QDesktopServices::openUrl(url);
}

void SyncClient::close()
{
    Q_EMIT
    this->prepareClose();
    qApp->quit();
}

void SyncClient::setProtocolCall(const bool &needCall)
{
    callLicenseDialog = needCall;
}

void SyncClient::JSLoadState(const bool isReady)
{
    if(isReady)
        Q_EMIT JSIsReady();
}

void SyncClient::setSession()
{
    Q_D(SyncClient);
    auto reply = d->daemonIf->Set(d->session);
    qDebug() << "set token with reply:" << reply.error();
}

void SyncClient::cleanSession()
{
    Q_D(SyncClient);
    auto reply = d->daemonIf->Set(QVariantMap());
    qDebug() << "clean token with reply:" << reply.error();
}

}
