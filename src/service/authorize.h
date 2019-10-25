#pragma once

#include <QStringList>

namespace ddc
{

class AuthorizeRequest
{
public:
    QString clientID;
    QStringList scopes;
    QString callback;
    QString state;
};

class AuthorizeResponse
{
public:
    bool success;
    QString code;
    QString state;

    AuthorizeRequest req;
};

}