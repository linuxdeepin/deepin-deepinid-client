#pragma once

#include <QObject>
#include <qcef_browser_event_delegate.h>

class WebEventDelegate : public QObject,
    public QCefBrowserEventDelegate
{
public:
    explicit WebEventDelegate(QObject *parent = nullptr);
    ~WebEventDelegate() override;

    bool onBeforeBrowse(const QUrl &url, bool is_redirect) override;

    void onBeforeContextMenu(QCefWebPage *web_page, QCefContextMenu *menu,
                             const QCefContextMenuParams &params) override;

    bool onBeforePopup(const QUrl &url,
                       QCefWindowOpenDisposition disposition) override;
};

