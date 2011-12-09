/*************************************************************************** 
** 
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies). 
** All rights reserved. 
** Contact: Nokia Corporation (testabilitydriver@nokia.com) 
** 
** This file is part of TDriver. 
** 
** If you have questions regarding the use of this file, please contact 
** Nokia at testabilitydriver@nokia.com . 
** 
** This library is free software; you can redistribute it and/or 
** modify it under the terms of the GNU Lesser General Public 
** License version 2.1 as published by the Free Software Foundation 
** and appearing in the file LICENSE.LGPL included in the packaging 
** of this file.
**
****************************************************************************/

#include <QObject>
#include <QString>

class TestabilityInterface
{
public:
     virtual ~TestabilityInterface() {}

     /*!
       Initializes the plugin once loaded.
     */
     virtual void Initialize() = 0;

};

 Q_DECLARE_INTERFACE(TestabilityInterface,
                     "com.nokia.testability.TestabilityInterface/1.0")

