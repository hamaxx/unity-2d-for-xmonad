/* This file is part of unity-2d
   Copyright 2009 Canonical
   Author: Aurelien Gateau <aurelien.gateau@canonical.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; version 3.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DEBUG_P_H
#define DEBUG_P_H

// Qt
#include <QDebug>

// STL
#include <string>

#define _UQ_TRACE(level) (level().nospace() << __PRETTY_FUNCTION__ << ":").space()

// Simple macros to get KDebug like support
#define UQ_DEBUG   _UQ_TRACE(qDebug)
#define UQ_WARNING _UQ_TRACE(qWarning)

// Log a variable name and value
#define UQ_VAR(var) UQ_DEBUG << #var ":" << var

#define UQ_RETURN_IF_FAIL(cond) if (!(cond)) { \
    UQ_WARNING << "Condition failed: " #cond; \
    return; \
}

#define UQ_RETURN_VALUE_IF_FAIL(cond, value) if (!(cond)) { \
    UQ_WARNING << "Condition failed: " #cond; \
    return (value); \
}

/**
 * A class to monitor entering and leaving blocks. Use it as is, or use the
 * convenience UQ_DEBUG_BLOCK macro which sets "from" to the current function.
 */
class Unity2dDebugBlock
{
public:
    Unity2dDebugBlock(const QString& from)
    : m_from(from)
    {
        qDebug() << "-->" << m_from;
    }

    ~Unity2dDebugBlock()
    {
        qDebug() << "<--" << m_from;
    }

private:
    QString m_from;
};

#define UQ_DEBUG_BLOCK Unity2dDebugBlock __unity2dDebugBlock__(__PRETTY_FUNCTION__)

// Support for outputing std::string with qDebug
QDebug operator<<(QDebug& dbg, const std::string& str);

#endif /* DEBUG_P_H */
