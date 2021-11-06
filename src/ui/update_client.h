#ifndef UPDATECLIENT_H
#define UPDATECLIENT_H

#include <QObject>

#include <com_deepin_lastore_jobmanager.h>
#include <com_deepin_lastore_job.h>

using ManagerInter = com::deepin::lastore::Manager;
using JobInter = com::deepin::lastore::Job;

namespace ddc
{

class UpdateClient : public QObject
{
    Q_OBJECT
public:
    explicit UpdateClient(QObject* parent = nullptr);
    ~UpdateClient();

    void checkForUpdate();

Q_SIGNALS:
    void updateFailed();

public Q_SLOTS:
    void onInstallPackage();

private:
    ManagerInter *m_managerInter = nullptr;
};
}
#endif // UPDATECLIENT_H
