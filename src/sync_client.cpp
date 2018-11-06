#include "sync_client.h"

#include <QDebug>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>

#include <QUrl>
#include <QDesktopServices>
#include <QCoreApplication>

namespace Const
{
const auto SyncDaemonService   = "com.deepin.sync.Daemon";
const auto SyncDaemonPath      = "/com/deepin/sync/Daemon";
const auto SyncDaemonInterface = "com.deepin.sync.Daemon";
}

namespace dsc
{

class SyncClientPrivate
{
public:
    SyncClientPrivate(SyncClient *parent) : q_ptr(parent)
    {
        daemonIf = new QDBusInterface(Const::SyncDaemonService,
                                      Const::SyncDaemonPath,
                                      Const::SyncDaemonInterface,
                                      QDBusConnection::sessionBus());
    }

    QDBusInterface *daemonIf;

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
    return d->daemonIf->property("MachineID").toString();
}

void SyncClient::setToken(const QString &token)
{
    Q_D(SyncClient);
    auto reply = d->daemonIf->call("SetToken", token);
    qDebug() << "set token" << token
             << "with reply:" << reply.errorName() << reply.errorMessage();

    // TODO: deal with failed issue
    qApp->quit();
}

void SyncClient::open(const QString &url)
{
    QDesktopServices::openUrl(url);
}

}
