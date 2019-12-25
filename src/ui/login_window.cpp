
#include "login_window.h"

#include <QDebug>
#include <QTimer>
#include <QUrl>
#include <QFile>
#include <QLocale>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusConnectionInterface>
#include <QtWebChannel/QWebChannel>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineScriptCollection>

#include <DTitlebar>
#include <DWidgetUtil>

#include "sync_client.h"
#include "service/authentication_manager.h"
#include "utils/utils.h"
#include "login_view.h"
#include "login_page.h"

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

        QDBusInterface activate("com.deepin.license.activator",
                                "/com/deepin/license/activator",
                                "com.deepin.license.activator",
                                QDBusConnection::sessionBus());

        QDBusReply<quint32> reply = activate.call(QDBus::AutoDetect,
                              "GetIndicatorData");
        authorizationState = reply.value();

        QObject::connect(&authMgr, &AuthenticationManager::requestLogin, parent, [=](const AuthorizeRequest &authReq)
        {
            // if need login,clean cookie;
            this->page->runJavaScript(
                "document.cookie.split(\";\").forEach(function(c) { document.cookie = c.replace(/^ +/, \"\").replace(/=.*/, \"=;expires=\" + new Date().toUTCString() + \";path=/\"); });");
            this->client.cleanSession();

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
            cancelAll();
        });
    }

    void cancelAll()
    {
        Q_Q(LoginWindow);
        authMgr.cancel();
        if (!hasLogin) {
            for (const auto &id: megs.keys()) {
                cancel(id);
            }
        }
        q->hide();
    }

    void cancel(const QString &clientID)
    {
        auto clientCallback = megs.value(clientID);
        qDebug() << clientID << clientCallback;
        if (nullptr == clientCallback) {
            qWarning() << "empty clientID" << clientID;
            return;
        }

        clientCallback->call(QDBus::Block, "OnCancel");
        qDebug() << "call" << clientCallback;

    }
    QString url;
    LoginPage *page;

    SyncClient client;
    AuthenticationManager authMgr;

    bool hasLogin = false;
    QMap<QString, QDBusInterface *> megs;

    LoginWindow *q_ptr;
    Q_DECLARE_PUBLIC(LoginWindow)

    unsigned int authorizationState;
};

LoginWindow::LoginWindow(QWidget *parent)
    : Dtk::Widget::DMainWindow(parent), dd_ptr(new LoginWindowPrivate(this))
{
    Q_D(LoginWindow);

    QFile scriptFile(":/qtwebchannel/qwebchannel.js");
    scriptFile.open(QIODevice::ReadOnly);
    QString apiScript = QString::fromLatin1(scriptFile.readAll());
    scriptFile.close();
    QWebEngineScript script;
    script.setSourceCode(apiScript);
    script.setName("qwebchannel.js");
    script.setWorldId(QWebEngineScript::MainWorld);
    script.setInjectionPoint(QWebEngineScript::DocumentCreation);
    script.setRunsOnSubFrames(false);
    QWebEngineProfile::defaultProfile()->scripts()->insert(script);

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

    auto machineID = d->client.machineID();
    auto *wuri = new WebUrlRequestInterceptor();
    wuri->setHeader({
                        {"X-Machine-ID", machineID.toLatin1()}
                    });
    QWebEngineProfile::defaultProfile()->setRequestInterceptor(wuri);

    auto *channel = new QWebChannel(this);
    channel->registerObject("client", &d->client);

    d->page = new LoginPage(this);
    d->page->setWebChannel(channel);


    connect(&d->client, &SyncClient::prepareClose, this, [&]()
    {
        this->close();
    });

    connect(&d->client, &SyncClient::requestHide, this, [&]()
    {
        this->hide();
    });

    auto view = new LoginView(this);
    view->setPage(d->page);
    this->setCentralWidget(view);
    view->setFocus();

    connect(d->page, &QWebEnginePage::loadStarted, this, [=]()
    {
        qDebug() << "ok";
    });
    connect(d->page, &QWebEnginePage::loadFinished, this, [=](bool ok)
    {
        qDebug() << ok;
        if (!ok) {
            d->page->load(QUrl("qrc:/web/error.html"));
        }
    });
    connect(d->page, &QWebEnginePage::loadProgress, this, [=](int progress)
    {
//        qDebug() << progress;
    });


    connect(this, &LoginWindow::loadError, this, &LoginWindow::onLoadError, Qt::QueuedConnection);

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
    d->page->load(QUrl(d->url));
}

void LoginWindow::Authorize(const QString &clientID,
                            const QStringList &scopes,
                            const QString &callback,
                            const QString &state)
{
    Q_D(LoginWindow);
    qDebug() << "d->reply:" << d->authorizationState;

    bool isNotAuthorized =  AuthorizationState::Notauthorized == d->authorizationState ||
                    AuthorizationState::Expired == d->authorizationState ||
                    AuthorizationState::TrialExpired == d->authorizationState;
    if(isNotAuthorized)
    {
        d->page->load(QUrl("qrc:/web/authorize.html"));
        show();
    }else{
        qDebug() << "requestAuthorize" << clientID << scopes << callback << state;
        d->authMgr.requestAuthorize(AuthorizeRequest{
            clientID, scopes, callback, state
        });
    }
}

void LoginWindow::onLoadError()
{
    Q_D(LoginWindow);
    qDebug() << "load error page";
    d->page->load(QUrl("qrc:/web/error.html"));
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
    d->cancelAll();
    QWidget::closeEvent(event);
}

}
