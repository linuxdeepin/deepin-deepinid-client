#include "login_window.h"

#include <QDebug>
#include <QUrl>
#include <QtWebChannel/QWebChannel>

#include <DTitlebar>

#include <qcef_web_page.h>
#include <qcef_web_settings.h>
#include <qcef_web_view.h>

#include "sync_client.h"

namespace dsc
{

class LoginWindowPrivate
{
public:
    LoginWindowPrivate(LoginWindow *parent) : q_ptr(parent)
    {
        client = new SyncClient(parent);
    }

    SyncClient *client;

    LoginWindow *q_ptr;
    Q_DECLARE_PUBLIC(LoginWindow)
};


LoginWindow::LoginWindow(QWidget *parent)
    : Dtk::Widget::DMainWindow(parent), dd_ptr(new LoginWindowPrivate(this))
{
    Q_D(LoginWindow);

    this->titlebar()->setTitle("");

    auto web_view_ = new QCefWebView();
    this->setCentralWidget(web_view_);
    qDebug() << web_view_;
    // Disable web security.
    auto settings = web_view_->page()->settings();
    settings->setMinimumFontSize(8);
    settings->setWebSecurity(QCefWebSettings::StateDisabled);


    auto web_channel = web_view_->page()->webChannel();

    web_channel->registerObject("client", d->client);
//    auto web_event_delegate_ = new dstore::WebEventDelegate(this);
//    web_view_->page()->setEventDelegate(web_event_delegate_);

    auto url = "https://login.deepin.org/oauth2/authorize?client_id=fcb9f8cac81074100b9482d534767a1fecc148b3&redirect_uri=https%3A%2F%2Faccount.deepin.org%2Flogin&response_type=code&scope=base%2Cuser%3Aread%2Cuser%3Aedit%2Cprofile%3Aread%2Cprofile%3Aedit&display=client";
//    url = "http://127.0.0.1:8000/a.html";
    web_view_->load(QUrl(url));
}

LoginWindow::~LoginWindow()
{

}
}
