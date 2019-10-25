#pragma once

#include <QObject>
#include <QScopedPointer>
#include "authorize.h"

namespace ddc
{
class SessionPrivate;
class Session: public QObject
{
Q_OBJECT
public:
    explicit Session(QObject *parent = Q_NULLPTR);
    ~Session() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void authorizeFinished(const AuthorizeResponse &);

public Q_SLOTS:
    void authorize(const AuthorizeRequest &authReq);

public:
    void save(const QString &sessionID);

private:
    QScopedPointer<SessionPrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), Session)
};

}
