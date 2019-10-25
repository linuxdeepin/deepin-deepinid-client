#include "session.h"

#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>

#include "utils/utils.h"

namespace ddc
{

QNetworkAccessManager manager;

class SessionPrivate
{
    explicit SessionPrivate(Session *parent)
        : q_ptr(parent)
    {}

    QSettings settings;

    Session *q_ptr;
    Q_DECLARE_PUBLIC(Session)
};

Session::Session(QObject *parent)
    : QObject(parent), dd_ptr(new SessionPrivate(this))
{

}

void Session::authorize(const AuthorizeRequest &authReq)
{
    Q_D(Session);
    auto url = utils::authCodeURL("/oauth2/client/authorize",
                                  authReq.clientID,
                                  authReq.scopes,
                                  authReq.callback,
                                  authReq.state);
    auto sessionID = d->settings.value("session");
    QNetworkRequest req(url);
    req.setRawHeader("X-DeepinID-SessionID", sessionID.toString().toLatin1());
    auto reply = manager.get(req);

    qDebug() << "get" << req.url();
    connect(reply, &QNetworkReply::finished, this, [=]()
    {
        AuthorizeResponse resp;
        resp.success = true;
        resp.req = authReq;

        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << reply->error();
            resp.success = false;
        }
        else {
            auto data = reply->readAll();
            qDebug() << "resp" << data;
            auto json = QJsonDocument::fromJson(data).object();
            resp.code = json.value("code").toString();
            resp.state = authReq.state;
            // TODO: handle error
            if (resp.code.isEmpty()) {
                resp.success = false;

            }
        }
        qDebug() << "resp" << resp.success;
        Q_EMIT this->authorizeFinished(resp);
    });
}

void Session::save(const QString &sessionID)
{
    Q_D(Session);
    d->settings.setValue("session", sessionID);
    d->settings.sync();
}

Session::~Session() = default;

}

