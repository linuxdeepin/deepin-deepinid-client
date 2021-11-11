#include "update_client.h"

using namespace ddc;

UpdateClient::UpdateClient(QObject *parent)
    : QObject (parent)
    , m_isInstartSuccess(false)
    , m_managerInter(new ManagerInter("com.deepin.lastore",
                                       "/com/deepin/lastore",
                                       QDBusConnection::systemBus(),
                                       this))
    , m_updaterInter(new UpdaterInter("com.deepin.lastore",
                                      "/com/deepin/lastore",
                                      QDBusConnection::systemBus(),
                                      this))
{
    connect(this, &UpdateClient::updateFinish, this, &UpdateClient::checkUpdatebleApps, Qt::QueuedConnection);
    connect(this, &UpdateClient::instartPackages, this, &UpdateClient::onInstallPackage, Qt::QueuedConnection);
}

UpdateClient::~UpdateClient()
{

}

void UpdateClient::checkForUpdate()
{
    qDebug() << " ++ Start update Client! ";
    QDBusPendingCall call = m_managerInter->UpdateSource();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (call.isError()) {
            qDebug() << " -- Client Check for update Error!" << call.error();
            return ;
        }
        QDBusReply<QDBusObjectPath> reply = call.reply();
        const QString jobPath = reply.value().path();

        QPointer<JobInter> checkUpdateJob = new JobInter("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);

        connect(checkUpdateJob, &JobInter::StatusChanged, [checkUpdateJob, this](const QString &status){
            if (status == "failed") {
                qDebug() << " -- Client check Failed!";
                if (checkUpdateJob) {
                    delete checkUpdateJob.data();
                }
            } else if (status == "success" || status == "succeed") {
                qDebug() << " ++ Client check Succeed";
                Q_EMIT this->updateFinish();
                if (checkUpdateJob) {
                    delete checkUpdateJob.data();
                }
            }
        });

    });
}

void UpdateClient::onInstallPackage()
{
    qDebug() << " ++ Install client! ";
    QDBusPendingCall call = this->m_managerInter->InstallPackage("client","deepin-deepinid-client");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (call.isError()) {
            qDebug() << " -- lient Install Error!" << call.error();
            return ;
        }

        QDBusReply<QDBusObjectPath> reply = call.reply();
        const QString jobPath = reply.value().path();
        QPointer<JobInter> installJob = new JobInter("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);

        connect(installJob, &JobInter::StatusChanged, [installJob, this](const QString &status){
            if (status == "failed") {
                qDebug() << " --  Client Install Failed!" << installJob.data();

                if (installJob) {
                    delete installJob.data();
                }
            } else if (status == "success" || status == "succeed") {
                m_isInstartSuccess = true;
                qDebug() << " ++ Client Install Succeed";
                if (installJob) {
                    delete installJob.data();
                }
            }
        });
    });
}

/**
 * @brief UpdateClient::checkUpdatebleApps
 */
void UpdateClient::checkUpdatebleApps()
{
    QList<QString> needUpdateApps = m_updaterInter->updatablePackages();
    for (int i = 0; i < needUpdateApps.count(); i++) {
        if (needUpdateApps.at(i).contains("deepin-deepinid-client")) {
            m_isInstartSuccess = false;
            qDebug() << " ++ need Instart Client ";
            Q_EMIT this->instartPackages();
        }
    }
}
