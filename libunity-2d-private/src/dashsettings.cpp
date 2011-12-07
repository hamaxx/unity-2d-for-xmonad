/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Alberto Mardegan <alberto.mardegan@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// Self
#include "dashsettings.h"

// Local

// libdconf-qt
#include "qconf.h"

// Qt
#include <QMetaEnum>
#include <QSize>
#include <QVariant>

static const int DASH_MIN_SCREEN_WIDTH = 1280;
static const int DASH_MIN_SCREEN_HEIGHT = 1084;

static const char* DASH_DCONF_SCHEMA = "com.canonical.Unity";
static const char* FORM_FACTOR = "formFactor";

namespace Unity2d {

class DashSettingsPrivate
{
    DashSettingsPrivate(DashSettings *dashSettings):
        conf(new QConf(DASH_DCONF_SCHEMA))
    {
        /* proxy the formFactor property signal notifier */
        QObject::connect(conf, SIGNAL(formFactorChanged(QString)),
                         dashSettings, SIGNAL(formFactorChanged()));

        /* Get the QMetaEnum for FormFactor: we will use it when setting and
         * getting the property, to convert the string value returned by QConf
         * to an enum value, and vice versa. */
        const QMetaObject *metaObject = dashSettings->metaObject();
        int index = metaObject->indexOfEnumerator("FormFactor");
        formFactorEnum = metaObject->enumerator(index);
    }

    ~DashSettingsPrivate()
    {
        delete conf;
    }

private:
    DashSettings* q_ptr;
    Q_DECLARE_PUBLIC(DashSettings)

    QConf* conf;
    QMetaEnum formFactorEnum;
};

DashSettings::DashSettings(QObject* parent):
    QObject(parent),
    d_ptr(new DashSettingsPrivate(this))
{
}

DashSettings::~DashSettings()
{
    delete d_ptr;
}

void DashSettings::setFormFactor(FormFactor formFactor)
{
    Q_D(DashSettings);

    const char* name = d->formFactorEnum.valueToKey(formFactor);
    d->conf->setProperty(FORM_FACTOR, QVariant(QString::fromLatin1(name)));
}

DashSettings::FormFactor DashSettings::formFactor() const
{
    Q_D(const DashSettings);
    QByteArray key = d->conf->property(FORM_FACTOR).toString().toLatin1();
    return FormFactor(d->formFactorEnum.keyToValue(key.constData()));
}

static int getenvInt(const char* name, int defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    int value = stringValue.toInt(&ok);
    return ok ? value : defaultValue;
}

QSize DashSettings::minimumSizeForDesktop()
{
    static int minWidth = getenvInt("DASH_MIN_SCREEN_WIDTH",
                                    DASH_MIN_SCREEN_WIDTH);
    static int minHeight = getenvInt("DASH_MIN_SCREEN_HEIGHT",
                                     DASH_MIN_SCREEN_HEIGHT);
    return QSize(minWidth, minHeight);
}

}; // namespace Unity2d

#include "dashsettings.moc"
