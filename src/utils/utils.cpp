
#include "utils.h"

#include <QLocale>

namespace utils
{

QString authCodeURL(const QString &clientID,
                    const QStringList &scopes,
                    const QString &callback,
                    const QString &state)
{
    QString templateURL =
        "%1/oauth2/authorize?client_id=%2&redirect_uri=%3&response_type=code&scope=%4&display=sync&handle_open_link=true&lang=%5";
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
        arg("zh_cn");
    return url;
}

QString authCodeURL(const QString &path,
                    const QString &clientID,
                    const QStringList &scopes,
                    const QString &callback,
                    const QString &state)
{
    QString templateURL =
        "%1%2?client_id=%3&redirect_uri=%4&response_type=code&scope=%5&display=sync&handle_open_link=true&lang=%6";
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
        arg(locale);
    return url;
}
};