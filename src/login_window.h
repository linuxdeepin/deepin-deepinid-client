#pragma once

#include <QtCore/qglobal.h>
#include <DMainWindow>

namespace dsc
{
class LoginWindowPrivate;
class LoginWindow : public Dtk::Widget::DMainWindow
{
Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.deepinid.Client")
public:
    explicit LoginWindow(QWidget *parent = Q_NULLPTR);
    ~LoginWindow() Q_DECL_OVERRIDE;

    bool isLogin() const;
    void setURL(const QString &url);
    void load();

public Q_SLOTS:
    Q_SCRIPTABLE void Show();

private:
    QScopedPointer<LoginWindowPrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), LoginWindow)
};

}
