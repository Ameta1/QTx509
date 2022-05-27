#include "settingsmap.h"

SettingsMap::SettingsMap()
{
    settingsMap.clear();
    settingList.clear();
}

void SettingsMap::insert(const QString& setting, const QString& value)
{
    if (settingsMap.contains(setting)) {
        settingsMap.insert(setting, value);
    }
    else {
        settingList.append(settingsMap.insert(setting, value));
    }
}

QString SettingsMap::value(const QString& key) const
{
    return settingsMap.value(key);
}

QPair<QString, QString> SettingsMap::at(int index) const
{
    return QPair<QString, QString>(settingList.at(index).key(), settingList.at(index).value());
}

int SettingsMap::size() const
{
    return settingsMap.size();
}
