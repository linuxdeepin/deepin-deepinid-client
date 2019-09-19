#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWebEngineView>

class LoginView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit LoginView(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

};

#endif // LOGINVIEW_H
