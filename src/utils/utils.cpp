
#include "utils.h"

#include <QLocale>

namespace utils
{

QString authCodeURL(const QString &clientID,
                    const QStringList &scopes,
                    const QString &callback,
                    const QString &state)
{
    QString templateURL = "%1/oauth2/authorize";
    templateURL += "?response_type=code";
    templateURL += "&client_id=%3";
    templateURL += "&redirect_uri=%4";
    templateURL += "&scope=%5";
    templateURL += "&state=%6";
    templateURL += "&lang=%7";
    templateURL += "&display=sync";
    templateURL += "&version=2.0";
    templateURL += "&handle_open_link=true";

    QString oauthURI = "https://login.deepinid.deepin.com";
    QString locale = QLocale().name().split("_").value(0);

    if (!qEnvironmentVariableIsEmpty("DEEPINID_OAUTH_URI")) {
        oauthURI = qgetenv("DEEPINID_OAUTH_URI");
    }

    auto url = QString(templateURL).
        arg(oauthURI).
        arg(clientID).
        arg(callback).
        arg(scopes.join(",")).
        arg(state).
        arg("zh_cn");
    return url;
}

QString authCodeURL(const QString &path,
                    const QString &clientID,
                    const QStringList &scopes,
                    const QString &callback,
                    const QString &state)
{
    QString templateURL = "%1%2";
    templateURL += "?response_type=code";
    templateURL += "&client_id=%3";
    templateURL += "&redirect_uri=%4";
    templateURL += "&scope=%5";
    templateURL += "&state=%6";
    templateURL += "&lang=%7";
    templateURL += "&display=sync";
    templateURL += "&version=2.0";
    templateURL += "&handle_open_link=true";
    QString oauthURI = "https://login.deepinid.deepin.com";
    QString locale = QLocale().name().split("_").value(0);

    if (!qEnvironmentVariableIsEmpty("DEEPINID_OAUTH_URI")) {
        oauthURI = qgetenv("DEEPINID_OAUTH_URI");
    }

    auto url = QString(templateURL).
        arg(oauthURI).
        arg(path).
        arg(clientID).
        arg(callback).
        arg(scopes.join(",")).
        arg(state).
        arg(locale);
    return url;
}
};
