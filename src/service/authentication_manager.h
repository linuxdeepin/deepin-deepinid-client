#pragma once

#include <QObject>
#include <QScopedPointer>

#include "authorize.h"

namespace ddc
{

class AuthenticationManagerPrivate;
class AuthenticationManager: public QObject
{
Q_OBJECT
public:
    explicit AuthenticationManager(QObject *parent = Q_NULLPTR);
    ~AuthenticationManager() Q_DECL_OVERRIDE;

    bool hasRequest() const;
Q_SIGNALS:
    void authorizeFinished(const AuthorizeResponse &);
    void requestLogin(const AuthorizeRequest &authReq);

public Q_SLOTS:
    void requestAuthorize(const AuthorizeRequest &authReq);
    void onLogin(const QString &sessionID,
                 const QString &clientID,
                 const QString &code,
                 const QString &state);

private:
    QScopedPointer<AuthenticationManagerPrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), AuthenticationManager)
};

}
