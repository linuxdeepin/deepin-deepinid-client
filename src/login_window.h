#pragma once

#include <QtCore/qglobal.h>
#include <DMainWindow>

namespace dsc {

class LoginWindowPrivate;
class LoginWindow: public Dtk::Widget::DMainWindow
{
public:
    LoginWindow(QWidget *parent = Q_NULLPTR);
    ~LoginWindow();

    void setURL(const QString& url);
    void load();

private:
    QScopedPointer<LoginWindowPrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), LoginWindow)
};

}
