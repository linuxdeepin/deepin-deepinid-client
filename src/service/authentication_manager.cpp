#include "authentication_manager.h"

#include "session.h"

#include <QDebug>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>

namespace ddc
{

class AuthenticationManagerPrivate
{
public:
    explicit AuthenticationManagerPrivate(AuthenticationManager *p)
        : q_ptr(p)
    {
        ddc::Session::connect(&sess, &Session::authorizeFinished, p, [=](const AuthorizeResponse &resp)
        {
//            QMutexLocker locker(&mutex);

            if (this->authQueue.isEmpty()) {
                qCritical() << "should not empty";
                return;
            }

            // remove from queue;
            if (resp.success) {
                this->authQueue.pop_front();
                qDebug() << "authorizeFinished" << this->authQueue.length();
                Q_EMIT p->authorizeFinished(resp);
            }
            else {
                auto authReq = this->authQueue.first();
                Q_EMIT p->requestLogin(authReq);
            }
        });
    }

    Session sess;
    QMutex mutex;

    QQueue<AuthorizeRequest> authQueue;

    AuthenticationManager *q_ptr;
    Q_DECLARE_PUBLIC(AuthenticationManager)
};

AuthenticationManager::AuthenticationManager(QObject *parent)
    : QObject(parent), dd_ptr(new AuthenticationManagerPrivate(this))
{

}

void AuthenticationManager::requestAuthorize(const AuthorizeRequest &authReq)
{
    // lock and wait
    // push req to queue, and deal when login
    Q_D(AuthenticationManager);
//    QMutexLocker locker(&d->mutex);
    d->authQueue.push_back(authReq);

    qDebug() << "queue length" << d->authQueue.length();
    // first req, call authorize
    if (d->authQueue.length() == 1) {
        d->sess.authorize(authReq);
    }
}

void AuthenticationManager::onLogin(const QString &sessionID,
                                    const QString &clientID,
                                    const QString &code,
                                    const QString &state)
{
    // save config
    Q_D(AuthenticationManager);
    // TODO: need crypt

//    QMutexLocker locker(&d->mutex);
    auto authReq = d->authQueue.first();
    if (authReq.clientID != clientID) {
        qCritical() << "error id" << authReq.clientID << clientID;
    }
    AuthorizeResponse resp;
    resp.success = true;
    resp.req = authReq;
    resp.state = authReq.state;
    resp.code = code;
    Q_EMIT d->sess.authorizeFinished(resp);
}

bool AuthenticationManager::hasRequest() const
{
    Q_D(const AuthenticationManager);
    return !d->authQueue.isEmpty();
}

void AuthenticationManager::cancel()
{
    Q_D(AuthenticationManager);
    d->authQueue.clear();
}

AuthenticationManager::~AuthenticationManager() = default;

}
