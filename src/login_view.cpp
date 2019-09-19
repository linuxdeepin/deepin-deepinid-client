#include "login_view.h"

#include <QContextMenuEvent>

LoginView::LoginView(QWidget *parent) : QWebEngineView(parent)
{

}

void LoginView::contextMenuEvent(QContextMenuEvent *event)
{
    event->ignore();
}
