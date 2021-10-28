#pragma once

#include <QStringList>

namespace utils
{

QString authCodeURL(const QString &clientID,
                    const QStringList &scopes,
                    const QString &callback,
                    const QString &state);

QString authCodeURL(const QString &path,
                    const QString &clientID,
                    const QStringList &scopes,
                    const QString &callback,
                    const QString &state);

QString getThemeName();

QString getActiveColor();

QString getStandardFont();

QString getDeviceKernel();

QString getDeviceProcessor();

QString getOsVersion();

QString getDeviceCode();

void sendDBusNotify(const QString &message);

QString getLang(const QString &region);

bool isTablet();

QString getDeviceType();

}
