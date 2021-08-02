#pragma once

#include <QString>

class Settings {
public:
    static QString deviceName();
    static QString downloadPath();
    static bool discoverable();
    static quint16 serverPort();
    static void setDeviceName(const QString &deviceName);
    static void setDownloadPath(const QString &downloadPath);
    static void setDiscoverable(bool discoverable);
    static void setServerPort(quint16 serverPort);
};
