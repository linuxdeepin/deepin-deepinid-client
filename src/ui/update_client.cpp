#include "update_client.h"

using namespace ddc;
static int m_tryAgainCun = 0;

UpdateClient::UpdateClient(QObject *parent)
    : QObject (parent)
    , m_managerInter(new ManagerInter("com.deepin.lastore",
                                       "/com/deepin/lastore",
                                       QDBusConnection::systemBus(),
                                       this))
{
    connect(this, &UpdateClient::updateFailed, this, &UpdateClient::onInstallPackage, Qt::QueuedConnection);
}

UpdateClient::~UpdateClient()
{

}

void UpdateClient::checkForUpdate()
{
    qDebug() << " ++ Start update Client! " << m_tryAgainCun;
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
                Q_EMIT this->updateFailed();
                if (checkUpdateJob) {
                    delete checkUpdateJob.data();
                }
            }
        });

    });
}

void UpdateClient::onInstallPackage()
{
    qDebug() << " ++ Install client! " << m_tryAgainCun;
    QDBusPendingCall call = this->m_managerInter->InstallPackage("client","deepin-deepinid-client");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (call.isError()) {
            qDebug() << " -- lient Install Error!" << call.error();
            if (m_tryAgainCun < 5) {
                checkForUpdate();
                m_tryAgainCun++;
            }
            return ;
        }

        QDBusReply<QDBusObjectPath> reply = call.reply();
        const QString jobPath = reply.value().path();
        QPointer<JobInter> installJob = new JobInter("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);

        connect(installJob, &JobInter::StatusChanged, [installJob, this](const QString &status){
            if (status == "failed") {
                qDebug() << " --  Client Install Failed!" << installJob.data();

                if (m_tryAgainCun <= 5) {
                    Q_EMIT this->updateFailed();
                    m_tryAgainCun++;
                }

                if (installJob) {
                    delete installJob.data();
                }
            } else if (status == "success" || status == "succeed") {
                qDebug() << " ++ Client Install Succeed";
                m_tryAgainCun = 0;
                if (installJob) {
                    delete installJob.data();
                }
            }
        });
    });
}
