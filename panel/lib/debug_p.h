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

#include <QDebug>

#define _UQ_BLUE  "\033[34m"
#define _UQ_RED   "\033[31m"
#define _UQ_RESET "\033[0m"
#define _UQ_TRACE(level, color) (level().nospace() << color << __PRETTY_FUNCTION__ << _UQ_RESET ":").space()

// Simple macros to get KDebug like support
#define UQ_DEBUG   _UQ_TRACE(qDebug, _UQ_BLUE)
#define UQ_WARNING _UQ_TRACE(qWarning, _UQ_RED)

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

#endif /* DEBUG_P_H */
