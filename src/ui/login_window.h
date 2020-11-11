#pragma once

#include <QtCore/qglobal.h>
#include <QDBusContext>
#include <QHostInfo>
#include <QDBusInterface>

#include <QNetworkConfigurationManager>
#include <DMainWindow>

namespace ddc
{
enum AuthorizationState
{
    Notauthorized,      //未授权
    Authorized,         //已授权
    Expired,            //已过期
    TrialAuthorized,    //试用期已授权
    TrialExpired        //试用期已过期
};

class LoginWindowPrivate;
class LoginWindow: public Dtk::Widget::DMainWindow,
                   protected QDBusContext
{
Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.deepinid.Client")
public:
    explicit LoginWindow(QWidget *parent = Q_NULLPTR);
    ~LoginWindow() Q_DECL_OVERRIDE;

    void setURL(const QString &url);
    void load();

Q_SIGNALS:
    void loadError();

public Q_SLOTS:
    void onLoadError();

public Q_SLOTS:
    Q_SCRIPTABLE void Register(const QString &clientID,
                               const QString &service,
                               const QString &path,
                               const QString &interface);

    Q_SCRIPTABLE void Authorize(const QString &clientID,
                                const QStringList &scopes,
                                const QString &callback,
                                const QString &state);

protected Q_SLOTS:
    void onLookupHost(QHostInfo host);
    void syncActiveColor(QString str, QMap<QString, QVariant> map, QStringList list);

protected:
    void closeEvent(QCloseEvent*) Q_DECL_OVERRIDE;

    bool windowloadingEnd = true;
    QDBusInterface *appearance_ifc_;

private:
    QScopedPointer<LoginWindowPrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), LoginWindow)
};

}
