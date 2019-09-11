
#include "login_window.h"

#include <QDebug>
#include <QTimer>
#include <QUrl>
#include <QLocale>
#include <QtWebChannel/QWebChannel>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineScriptCollection>
#include <QFile>

#include <DTitlebar>
#include <DWidgetUtil>

#include "sync_client.h"
#include "login_page.h"

namespace dsc
{

class LoginWindowPrivate
{
public:
    LoginWindowPrivate(LoginWindow *parent) : q_ptr(parent)
    {
        client = new SyncClient(parent);

        QString templateURL = "%1/oauth2/authorize?client_id=%2&redirect_uri=%3&response_type=code&scope=%4&display=sync&handle_open_link=true&lang=%5";
        QString oauthURI = "https://login.deepinid.deepin.com";
        QString clientID = "163296859db7ff8d72010e715ac06bdf6a2a6f87";
        QString redirectURI = "https://sync.deepinid.deepin.com/oauth/callback";
        QString scope = "base,user:read,sync,dstore";
        QString locale = QLocale().name().split("_").value(0);

        if (!qEnvironmentVariableIsEmpty("DEEPIN_DEEPINID_OAUTH_URI")) {
            oauthURI = qgetenv("DEEPIN_DEEPINID_OAUTH_URI");
        }

        if (!qEnvironmentVariableIsEmpty("DEEPIN_DEEPINID_REDIRECT_URI")) {
            redirectURI = qgetenv("DEEPIN_DEEPINID_REDIRECT_URI");
        }

        url = QString(templateURL).arg(oauthURI).arg(clientID).arg(redirectURI).arg(scope).arg(locale);
    }

    LoginPage       *page;
    SyncClient      *client;
    QString         url;

    LoginWindow *q_ptr;
    Q_DECLARE_PUBLIC(LoginWindow)
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


    auto machineID = d->client->machineID();

    WebUrlRequestInterceptor *wuri = new WebUrlRequestInterceptor();
    wuri->setHeader({
        {"X-Machine-ID", machineID.toLatin1()}
    });
    QWebEngineProfile::defaultProfile()->setRequestInterceptor(wuri);


    // TODO: support error page
    //    auto delegate = new WebEventDelegate(this);
    //    d->webView->page()->setEventDelegate(delegate);
    //    qDebug() << d->webView->page()->pageErrorContent();
    //    d->webView->page()->setPageErrorContent("<script>window.location.href='qrc:/web/error.html';</script>");

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject("client", d->client);

    d->page = new LoginPage(this);
    d->page->setWebChannel(channel);

    connect(d->client, &SyncClient::prepareClose, this, [&]() {
        this->close();
    });

    connect(d->client, &SyncClient::requestHide, this, [&]() {
        this->close();
    });

    auto view = new QWebEngineView(this);
    view->setPage(d->page);

    this->setCentralWidget(view);

    QTimer::singleShot(100, this, SLOT(setFocus()));
}

LoginWindow::~LoginWindow()
{

}

bool LoginWindow::logined() const
{
    Q_D(const LoginWindow);
    return d->client->logined();
}

void LoginWindow::Show()
{
    this->show();
    Dtk::Widget::moveToCenter(this);
    this->load();
}

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

}
