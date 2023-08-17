// SPDX-FileCopyrightText: 2018-2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtCore/qglobal.h>
#include <DMainWindow>

namespace dsc
{
class LoginWindowPrivate;
class LoginWindow: public Dtk::Widget::DMainWindow
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.deepinid.Client")
public:
    LoginWindow(QWidget *parent = Q_NULLPTR);
    ~LoginWindow();

    bool logined() const;
    void setURL(const QString &url);
    void load();

Q_SIGNALS:
    void loadError();

public Q_SLOTS:
    void onLoadError();

    Q_SCRIPTABLE void Show();

private:
    QScopedPointer<LoginWindowPrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), LoginWindow)
};

}
