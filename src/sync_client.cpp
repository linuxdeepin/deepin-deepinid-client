#include "sync_client.h"

#include <QDebug>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>

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

void SyncClient::setToken(const QString &token)
{
//    Q_D(SyncClient);
    qDebug() << token;
//   d->daemonIf->call();
}
}
