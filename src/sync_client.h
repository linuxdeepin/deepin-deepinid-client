#pragma once

#include <QObject>
#include <QScopedPointer>

namespace dsc
{

class SyncClientPrivate;
class SyncClient : public QObject
{
    Q_OBJECT
public:
    explicit SyncClient(QObject *parent = Q_NULLPTR);
    ~SyncClient();

    QString machineID() const;
    bool logined() const;

public Q_SLOTS:
    Q_SCRIPTABLE void setToken(const QString &token);
    Q_SCRIPTABLE void open(const QString &url);

private:
    QScopedPointer<SyncClientPrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), SyncClient)
};

}
