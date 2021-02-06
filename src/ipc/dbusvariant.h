/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DBUSVARIANT
#define DBUSVARIANT

#include <QtCore>
#include <QtDBus>
#include <QtDBus/QDBusArgument>

class Inhibit
{
public:
    QString what;
    QString who;
    QString why;
    QString mode;
    quint32 uid;
    quint32 pid;

    Inhibit();
    ~Inhibit();

    friend QDBusArgument &operator<<(QDBusArgument &argument, const Inhibit &obj);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Inhibit &obj);
    static void registerMetaType();
};

class UsrInfo
{
public:
    qlonglong pid;
    QString id;
    QString userName;

    UsrInfo();
    ~UsrInfo();

    friend QDBusArgument &operator<<(QDBusArgument &argument, const UsrInfo &obj);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, UsrInfo &obj);
    static void registerMetaType();
};

class SeatInfo
{
public:
    QString id;
    QString seat;

    SeatInfo();
    ~SeatInfo();

    friend QDBusArgument &operator<<(QDBusArgument &argument, const SeatInfo &obj);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, SeatInfo &obj);
    static void registerMetaType();
};

class SessionInfo
{
public:
    QString session;
    qlonglong pid;
    QString user;
    QString id;
    QString seat;

    SessionInfo();
    ~SessionInfo();

    friend QDBusArgument &operator<<(QDBusArgument &argument, const SessionInfo &obj);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, SessionInfo &obj);
    static void registerMetaType();
};

typedef QList<Inhibit> InhibitorsList;
Q_DECLARE_METATYPE(Inhibit)
Q_DECLARE_METATYPE(InhibitorsList)

typedef QList<UsrInfo> UserList;
Q_DECLARE_METATYPE(UsrInfo)
Q_DECLARE_METATYPE(UserList)

typedef QList<SeatInfo> SeatList;
Q_DECLARE_METATYPE(SeatInfo)
Q_DECLARE_METATYPE(SeatList)

typedef QList<SessionInfo> SessionList;
Q_DECLARE_METATYPE(SessionInfo)
Q_DECLARE_METATYPE(SessionList)
#endif // DBUSVARIANT
