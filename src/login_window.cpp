
#include "login_window.h"

#include <QDebug>
#include <QUrl>
#include <QtWebChannel/QWebChannel>

#include <DTitlebar>
#include <DWidgetUtil>

#include <qcef_web_page.h>
#include <qcef_web_settings.h>
#include <qcef_web_view.h>

#include "sync_client.h"
#include "web_event_delegate.h"

namespace dsc
{

class LoginWindowPrivate
{
public:
    LoginWindowPrivate(LoginWindow *parent) : q_ptr(parent)
    {
        client = new SyncClient(parent);

        auto templateURL = "http://login.deepin.org/oauth2/authorize?client_id=%1&redirect_uri=%2&response_type=code&scope=%3&display=sync&handle_open_link=true";
        auto clientID = "163296859db7ff8d72010e715ac06bdf6a2a6f87";
        auto redirectURI = "http://sync.deepin.org/oauth/callback";
        auto scope = "base,user:read";
        url = QString(templateURL).arg(clientID).arg(redirectURI).arg(scope);
    }

    QCefWebView     *webView;
    SyncClient      *client;
    QString         url;

    LoginWindow *q_ptr;
    Q_DECLARE_PUBLIC(LoginWindow)
};


LoginWindow::LoginWindow(QWidget *parent)
    : Dtk::Widget::DMainWindow(parent), dd_ptr(new LoginWindowPrivate(this))
{
    Q_D(LoginWindow);

    this->titlebar()->setTitle("");
    setWindowFlag(Qt::Dialog);

    auto flag = windowFlags();
    flag &= ~Qt::WindowMinMaxButtonsHint;
    setWindowFlags(flag);
    this->titlebar()->setDisableFlags(Qt::WindowSystemMenuHint);
    this->titlebar()->setBackgroundTransparent(true);

    d->webView = new QCefWebView();
    this->setCentralWidget(d->webView);

    auto machineID = d->client->machineID();
    // Disable web security.
    auto settings = d->webView->page()->settings();
    settings->setMinimumFontSize(8);
    settings->setWebSecurity(QCefWebSettings::StateDisabled);
    settings->setCustomHeaders({
        {"X-Machine-ID", machineID}
    });

    auto delegate = new WebEventDelegate(this);
    d->webView->page()->setEventDelegate(delegate);

    auto web_channel = d->webView->page()->webChannel();
    web_channel->registerObject("client", d->client);
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
    d->webView->load(QUrl(d->url));
}

}
