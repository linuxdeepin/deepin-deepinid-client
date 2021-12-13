
#include "utils.h"

#include <QLocale>
#include <DGuiApplicationHelper>
#include <QDBusInterface>

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
    templateURL += "&theme=%8";
    templateURL += "&color=%9";
    templateURL += "&font_family=%10";
    templateURL += "&display=sync";
    templateURL += "&version=2.0";


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
        arg(locale).
        arg(getThemeName()).
        arg(getActiveColor()).
        arg(getStandardFont());
    return url.remove(QRegExp("#"));
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
    templateURL += "&theme=%8";
    templateURL += "&color=%9";
    templateURL += "&font_family=%10";
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
        arg(locale).
        arg(getThemeName()).
        arg(getActiveColor()).
        arg(getStandardFont());

    return url.remove(QRegExp("#"));
}

QString getThemeName()
{
    auto themeType = Dtk::Gui::DGuiApplicationHelper::instance()->themeType();

    switch (themeType) {
    case Dtk::Gui::DGuiApplicationHelper::DarkType:
        return "dark";
    case Dtk::Gui::DGuiApplicationHelper::LightType:
    case Dtk::Gui::DGuiApplicationHelper::UnknownType:
    default:
        return "light";
    }
}

QString getActiveColor()
{
    QDBusInterface appearance_ifc_(
                "com.deepin.daemon.Appearance",
                "/com/deepin/daemon/Appearance",
                "com.deepin.daemon.Appearance",
                QDBusConnection::sessionBus()
                );
    qDebug() << "ActiveColor" << appearance_ifc_.isValid();
    return appearance_ifc_.property("QtActiveColor").toString();
}

QString getStandardFont(){
    QDBusInterface appearance_ifc_(
                "com.deepin.daemon.Appearance",
                "/com/deepin/daemon/Appearance",
                "com.deepin.daemon.Appearance",
                QDBusConnection::sessionBus()
                );
    qDebug() << "StandardFont" << appearance_ifc_.isValid() << appearance_ifc_.property("StandardFont").toString();
    return appearance_ifc_.property("StandardFont").toString().remove(QRegExp("#"));
}

};
