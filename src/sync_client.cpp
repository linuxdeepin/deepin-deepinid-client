#include "sync_client.h"

#include <QDebug>
#include <QtDBus>
#include <QVariantMap>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>

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

void SyncClient::setToken(const QVariantMap &tokenInfo)
{
    Q_D(SyncClient);
    auto reply = d->daemonIf->SetToken(tokenInfo);
    qDebug() << "set token" << tokenInfo
             << "with reply:" << reply.error();

    // TODO: deal with failed issue
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
