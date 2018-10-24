#pragma once

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

namespace dsc
{

class SyncClientPrivate;
class SyncClient : public QObject
{
    Q_OBJECT
public:
    explicit SyncClient(QObject *parent = Q_NULLPTR);
    ~SyncClient();

public Q_SLOTS:
    Q_SCRIPTABLE void setToken(const QString& token);

private:
    QScopedPointer<SyncClientPrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), SyncClient)
};

}
