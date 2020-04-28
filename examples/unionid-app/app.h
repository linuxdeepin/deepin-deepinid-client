#ifndef APP_H
#define APP_H

#include <QObject>

class App : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.deepinid.App")
public:
    explicit App(QObject *parent = nullptr);

public Q_SLOTS:
    // Just for test
    Q_SCRIPTABLE void TestStartAuthorize();

public Q_SLOTS:
    Q_SCRIPTABLE void OnAuthorized(const QString &code, const QString &state);
    Q_SCRIPTABLE void OnCancel();
};

#endif // APP_H
