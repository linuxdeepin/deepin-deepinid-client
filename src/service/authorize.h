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

    bool operator==(const AuthorizeRequest &ps) const
    {
      if (this->clientID==ps.clientID)
         return true;
      return false;
    }
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
