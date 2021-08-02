#include "settings.h"
#include <QApplication>
#include <QDir>
#include <QHostInfo>
#include <QSettings>
#include <QStandardPaths>
QString Settings::deviceName()
{
    QString d = QHostInfo::localHostName();
    return QSettings().value("deviceName", d).toString();
}

QString Settings::downloadPath()
{
    QString d = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    d += QDir::separator() + QApplication::applicationName();
    return QSettings().value("downloadPath", d).toString();
}

bool Settings::discoverable()
{
    return QSettings().value("discoverable", true).toBool();
}

quint16 Settings::serverPort()
{
    return QSettings().value("serverPort", 0u).toUInt();
}

void Settings::setDeviceName(const QString &deviceName)
{
    QSettings().setValue("deviceName", deviceName);
}

void Settings::setDownloadPath(const QString &downloadPath)
{
    QSettings().setValue("downloadPath", downloadPath);
}

void Settings::setDiscoverable(bool discoverable)
{
    QSettings().setValue("discoverable", discoverable);
}

void Settings::setServerPort(quint16 port)
{
    QSettings().setValue("serverPort", port);
}
