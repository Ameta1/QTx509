#ifndef SETTINGSMAP_H
#define SETTINGSMAP_H

#include <QString>
#include <QHash>
#include <QModelIndex>

class SettingsMap
{
public:
    SettingsMap();
    void insert(const QString& setting, const QString& value);
    QString value(const QString& key) const;
    QPair<QString, QString> at(int index) const;
    int size() const;
private:
    QHash<QString, QString> settingsMap;
    QList<QHash<QString, QString>::Iterator> settingList;
};

#endif // SETTINGSMAP_H
