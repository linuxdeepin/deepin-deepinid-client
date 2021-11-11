#ifndef UPDATECLIENT_H
#define UPDATECLIENT_H

#include <QObject>

#include <com_deepin_lastore_jobmanager.h>
#include <com_deepin_lastore_updater.h>
#include <com_deepin_lastore_job.h>

using ManagerInter = com::deepin::lastore::Manager;
using JobInter = com::deepin::lastore::Job;
using UpdaterInter = com::deepin::lastore::Updater;

namespace ddc
{

class UpdateClient : public QObject
{
    Q_OBJECT
public:
    explicit UpdateClient(QObject* parent = nullptr);
    ~UpdateClient();

    inline  bool isInstartSuccess() { return m_isInstartSuccess; }

    void checkForUpdate();

Q_SIGNALS:
    void updateFinish();
    void instartPackages();

private Q_SLOTS:
    void onInstallPackage();
    void checkUpdatebleApps();

private:
    bool m_isInstartSuccess;
    ManagerInter *m_managerInter = nullptr;
    UpdaterInter *m_updaterInter = nullptr;
};
}
#endif // UPDATECLIENT_H
