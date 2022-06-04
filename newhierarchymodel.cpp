#include <QFile>
#include <QDir>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileInfo>

#include "newhierarchymodel.h"

newHierarchyModel::newHierarchyModel(QObject *parent)
    : QAbstractListModel(parent), midlayer()
{
}

int newHierarchyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return midlayer.size();
}

QHash<int, QByteArray> newHierarchyModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { NameRole, "Name" },
        { ValueRole, "Value"}
    };
    return roles;
}

QVariant newHierarchyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < rowCount())
        switch (role) {
        case NameRole: return midlayer.settingsList.at(index.row()).first;
        case ValueRole: return midlayer.settingsList.at(index.row()).second;
        default: return QVariant();
    }
    return QVariant();
}

void newHierarchyModel::newData(settingsContainer azaza)
{
    midlayer.settingsList.append(QPair<QString, QVariant>("govno", azaza.country));
}

void newHierarchyModel::createRoot()
{
    if (!midlayer.generateRootCertificate())
    {
        emit certListChanged();
        emit rootCreatedChanged();
    }
}

void newHierarchyModel::createRootAndIntermediate()
{
    if (!midlayer.generateRootCertificate()) {
        emit certListChanged();
        emit rootCreatedChanged();
    }
    if (!midlayer.generateIntermediateCertificate(midlayer.findSetting("intermediateCASuffix").toString())) {
        emit certListChanged();
    }
}

void newHierarchyModel::createAdditionalIntermediate(QString identificator)
{
    if (!midlayer.generateIntermediateCertificate(identificator)) {
        emit certListChanged();
    }
}

void newHierarchyModel::createLeaf(QString identificator, QString ancestorFile)
{
    if (!midlayer.generateLeafCertificate(identificator, ancestorFile)) {
        emit certListChanged();
    }
}

bool newHierarchyModel::isRootCreated()
{
    return QFile(midlayer.rootCertFilename()).exists();
}

void newHierarchyModel::save()
{
    QJsonObject settings;

    settings.insert("country", midlayer.findSetting("country").toString());
    settings.insert("province", midlayer.findSetting("province").toString());
    settings.insert("city", midlayer.findSetting("city").toString());
    settings.insert("organization", midlayer.findSetting("organization").toString());
    settings.insert("common", midlayer.findSetting("common").toString());
    settings.insert("cypherSuite", midlayer.findSetting("cypherSuite").toString());
    settings.insert("daysValid",midlayer.findSetting("daysValid").toInt());
    settings.insert("rootCAsuffix", midlayer.findSetting("rootCAsuffix").toString());
    if (midlayer.findSetting("threeLevels").toBool()) {
        settings.insert("intermediateCASuffix", midlayer.findSetting("intermediateCASuffix").toString());
    }

    QJsonDocument doc(settings);

    QString hierarchyPath = midlayer.findSetting("folder").toString() + "/" + "openssl_root_ca_config.json";
    QFile configFile(hierarchyPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        emit openErrorPopup();
        return;
    }
    configFile.write(doc.toJson());
    configFile.close();
}

void newHierarchyModel::load(QString file)
{
    QFile configFile(QUrl(file).toLocalFile());
    if (!configFile.open(QIODevice::ReadOnly)) {
        return;
    }
    QJsonParseError err {};
    QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        return;
    }
    configFile.close();

    QJsonObject settings = doc.object();
    beginResetModel();
    for (const auto& setting : settings.keys()) {
        if (midlayer.changeSetting(setting, settings.value(setting).toString())) {
            emit openErrorPopup();
            return;
        }
    }
    if (midlayer.changeSetting("folder", QFileInfo(QUrl(file).toLocalFile()).absolutePath())) {
        emit openErrorPopup();
    }
}

void newHierarchyModel::createNew()
{
    QString folder = midlayer.findSetting("folder").toString();
    QDir().mkpath(folder);
    QDir().mkdir(folder);

    save();

    QDir().mkdir(folder + "/public/");
    QDir().mkdir(folder + "/private/");
    QDir().mkdir(folder + "/certs/");
    QDir().mkdir(folder + "/csr/");
}
