
#include "login_window.h"

#include <QDebug>
#include <QTimer>
#include <QUrl>
#include <QLocale>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusConnectionInterface>
#include <QtWebChannel/QWebChannel>

#include <DTitlebar>
#include <DWidgetUtil>

#include <qcef_web_page.h>
#include <qcef_web_settings.h>
#include <qcef_web_view.h>

#include "sync_client.h"
#include "web_event_delegate.h"
#include "service/authentication_manager.h"
#include "utils/utils.h"

namespace ddc
{

class LoginWindowPrivate
{
public:
    explicit LoginWindowPrivate(LoginWindow *parent)
        : q_ptr(parent)
    {
        Q_Q(LoginWindow);
        qRegisterMetaType<AuthorizeRequest>("AuthorizeRequest");
        qRegisterMetaType<AuthorizeResponse>("AuthorizeResponse");

        QString clientID = "163296859db7ff8d72010e715ac06bdf6a2a6f87";
        QString redirectURI = "https://sync.deepinid.deepin.com/oauth/callback";
        QStringList scopes = {"base", "user:read", "sync", "dstore"};
        url = utils::authCodeURL(clientID, scopes, redirectURI, "");

        QObject::connect(&authMgr, &AuthenticationManager::requestLogin, parent, [=](const AuthorizeRequest &authReq)
        {
            url = utils::authCodeURL(
                authReq.clientID,
                authReq.scopes,
                authReq.callback,
                authReq.state);
            q->load();
            q->show();
        }, Qt::QueuedConnection);

        QObject::connect(&authMgr, &AuthenticationManager::authorizeFinished, parent, [=](const AuthorizeResponse &resp)
        {
            auto clientCallback = megs.value(resp.req.clientID);
            qDebug() << resp.req.clientID << clientCallback;
            if (nullptr == clientCallback) {
                qWarning() << "empty clientID" << resp.req.clientID;
                return;
            }

            clientCallback->call(QDBus::Block, "OnAuthorized", resp.code, resp.state);
            qDebug() << "call" << clientCallback << resp.code << resp.state;

            this->hasLogin = true;
            parent->hide();
            if (!authMgr.hasRequest()) {
                parent->close();
            }
        }, Qt::QueuedConnection);

        QObject::connect(&client, &SyncClient::onLogin, parent, [=](
            const QString &sessionID,
            const QString &clientID,
            const QString &code,
            const QString &state)
        {
            qDebug() << "on login";
            this->hasLogin = true;
            this->authMgr.onLogin(sessionID, clientID, code, state);
            this->client.setSession();
        }, Qt::QueuedConnection);

        QObject::connect(&client, &SyncClient::onCancel, parent, [=](
            const QString &clientID)
        {
            qDebug() << "onCancel";
            auto clientCallback = megs.value(clientID);
            qDebug() << clientID << clientCallback;
            if (nullptr == clientCallback) {
                qWarning() << "empty clientID" << clientID;
                return;
            }

            clientCallback->call(QDBus::Block, "OnCancel");
            qDebug() << "call" << clientCallback;
            parent->hide();
        });
    }

    QString url;
    QCefWebView *webView = nullptr;

    SyncClient client;
    AuthenticationManager authMgr;

    bool hasLogin = false;
    QMap<QString, QDBusInterface *> megs;

    LoginWindow *q_ptr;
    Q_DECLARE_PUBLIC(LoginWindow)
};

LoginWindow::LoginWindow(QWidget *parent)
    : Dtk::Widget::DMainWindow(parent), dd_ptr(new LoginWindowPrivate(this))
{
    Q_D(LoginWindow);

    this->titlebar()->setTitle("");
    setWindowFlags(Qt::Dialog);

    auto flag = windowFlags();
    flag &= ~Qt::WindowMinMaxButtonsHint;
    flag |= Qt::WindowStaysOnTopHint;
    setWindowFlags(flag);
    this->titlebar()->setMenuVisible(false);
    this->titlebar()->setMenuDisabled(true);

    // TODO: workaround for old version dtk, remove as soon as possible.
    this->titlebar()->setDisableFlags(Qt::WindowSystemMenuHint);

    this->titlebar()->setBackgroundTransparent(true);

    d->webView = new QCefWebView();
    this->setCentralWidget(d->webView);

    auto machineID = d->client.machineID();
    // Disable web security.
    auto settings = d->webView->page()->settings();
    settings->setMinimumFontSize(8);
    settings->setWebSecurity(QCefWebSettings::StateDisabled);
    settings->setCustomHeaders({
                                   {"X-Machine-ID", machineID}
                               });

    auto delegate = new WebEventDelegate(this);
    d->webView->page()->setEventDelegate(delegate);
    d->webView->page()->setPageErrorContent("<script>window.location.href='qrc:/web/error.html';</script>");
    auto web_channel = d->webView->page()->webChannel();
    web_channel->registerObject("client", &d->client);

    connect(&d->client, &SyncClient::prepareClose, this, [&]()
    {
        this->close();
    });

    connect(&d->client, &SyncClient::requestHide, this, [&]()
    {
        this->close();
    });

    setFixedSize(360, 390 + this->titlebar()->height());
    QTimer::singleShot(100, this, SLOT(setFocus()));
}

LoginWindow::~LoginWindow() = default;

void LoginWindow::setURL(const QString &url)
{
    Q_D(LoginWindow);
    d->url = url;
}

void LoginWindow::load()
{
    Q_D(LoginWindow);
    qDebug() << d->url;
    d->webView->load(QUrl(d->url));
}

void LoginWindow::Authorize(const QString &clientID,
                            const QStringList &scopes,
                            const QString &callback,
                            const QString &state)
{
    Q_D(LoginWindow);

    d->authMgr.requestAuthorize(AuthorizeRequest{
        clientID, scopes, callback, state
    });
}

void LoginWindow::Register(const QString &clientID,
                           const QString &service, const QString &path,
                           const QString &interface)
{
    Q_D(LoginWindow);
    // TODO: memory leak
    qDebug() << "register" << clientID << service << path << interface;
    auto dbusIfc = new QDBusInterface(service, path, interface);
    d->megs.insert(clientID, dbusIfc);
}

void LoginWindow::closeEvent(QCloseEvent *event)
{
    Q_D(LoginWindow);
    if (!d->hasLogin) {
        for (const auto &id: d->megs.keys()) {
            Q_EMIT d->client.onCancel(id);
        }
    }
    QWidget::closeEvent(event);
}

}
