#include "web_event_delegate.h"

#include <QDebug>
#include <QDesktopServices>

WebEventDelegate::WebEventDelegate(QObject *parent) :
    QObject(parent)
{
    this->setObjectName("WebEventDelegate");
}

WebEventDelegate::~WebEventDelegate()
{

}

bool WebEventDelegate::onBeforeBrowse(const QUrl &url, bool is_redirect)
{
    return QCefBrowserEventDelegate::onBeforeBrowse(url, is_redirect);
}

void WebEventDelegate::onBeforeContextMenu(QCefWebPage *web_page, QCefContextMenu *menu, const QCefContextMenuParams &params)
{
    return QCefBrowserEventDelegate::onBeforeContextMenu(web_page, menu, params);
}

bool WebEventDelegate::onBeforePopup(const QUrl &url, QCefWindowOpenDisposition disposition)
{
    return QCefBrowserEventDelegate::onBeforePopup(url, disposition);
}
