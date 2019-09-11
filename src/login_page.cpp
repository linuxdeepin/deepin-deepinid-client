#include "login_page.h"

void WebUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    for (auto k : header.keys()) {
        info.setHttpHeader(k, header[k]);
    }
}

