#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>

#include "qfile.h"
#include "hierarchymodel.h"
#include "sslfunctions.h"

HierarchyModel::HierarchyModel(QObject *parent ) : QObject(parent), midlayer()
{
    midlayer.threelevels = false;
}

void HierarchyModel::save()
{
    QJsonObject settings;
    settings.insert("country", midlayer.country);
    settings.insert("province", midlayer.province);
    settings.insert("city", midlayer.city);
    settings.insert("organization", midlayer.organization);
    settings.insert("common", midlayer.common);
    settings.insert("cypherSuite", midlayer.cypherSuite);
    settings.insert("daysValid", midlayer.daysValid);
    settings.insert("rootCAsuffix", midlayer.rootCAsuffix);
    if (midlayer.threelevels) {
        settings.insert("intermediateCASuffix", midlayer.intermediateCASuffix);
    }

    QJsonDocument doc(settings);

    QString hierarchyPath = midlayer.folder + "/" + "openssl_root_ca_config.json";
    QFile configFile(hierarchyPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        //ALARM
             return;
    configFile.write(doc.toJson());
    configFile.close();
}

void HierarchyModel::createNew()
{
    QDir().mkpath(midlayer.folder);
    QDir().mkdir(midlayer.folder);

    this->save();

    QDir().mkdir(midlayer.folder + "/public/");
    QDir().mkdir(midlayer.folder + "/private/");
    QDir().mkdir(midlayer.folder + "/certs/");
    QDir().mkdir(midlayer.folder + "/csr/");
}

void HierarchyModel::createRoot()
{
    if (!midlayer.generateRootCertificate())
    {
        emit certListChanged();
        emit rootCreatedChanged();
    }
}

void HierarchyModel::createRootAndIntermediate()
{
    if (!midlayer.generateRootCertificate()) {
        emit certListChanged();
        emit rootCreatedChanged();
    }
    if (!midlayer.generateIntermediateCertificate(midlayer.intermediateCASuffix)) {
        emit certListChanged();
    }
}

void HierarchyModel::createAdditionalIntermediate(QString identificator)
{
    if (!midlayer.generateIntermediateCertificate(identificator)) {
        emit certListChanged();
    }
}

void HierarchyModel::createLeaf(QString identificator, QString ancestorFile)
{
    if (!midlayer.generateLeafCertificate(identificator, ancestorFile)) {
        emit certListChanged();
    }
}

bool HierarchyModel::isRootCreated()
{
    return QFile(midlayer.rootCertFilename()).exists();
}

void HierarchyModel::load(QString file)
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

    midlayer.country = settings.value("country").toString();
    emit countryChanged();
    midlayer.province = settings.value("province").toString();
    emit provinceChanged();
    midlayer.city = settings.value("city").toString();
    emit cityChanged();
    midlayer.organization = settings.value("organization").toString();
    emit organizationChanged();
    midlayer.common = settings.value("common").toString();
    emit commonChanged();
    midlayer.cypherSuite = settings.value("cypherSuite").toString();
    emit cypherSuiteChanged();
    midlayer.daysValid = settings.value("daysValid").toString();
    emit daysValidChanged();
    midlayer.rootCAsuffix = settings.value("rootCAsuffix").toString();
    emit rootCAsuffixChanged();

    if (!settings.value("intermediateCASuffix").isUndefined()) {
        midlayer.intermediateCASuffix = settings.value("intermediateCASuffix").toString();
        emit intermediateCASuffixChanged();
        midlayer.threelevels = true;
        emit threelevelsChanged();
    }
    else {
        midlayer.threelevels = false;
        emit threelevelsChanged();
    }

    midlayer.folder = QFileInfo(QUrl(file).toLocalFile()).absolutePath();
    emit certListChanged();
}

const QString &HierarchyModel::getFolder() const
{
    return midlayer.folder;
}

void HierarchyModel::setFolder(const QString &newFolder)
{
    midlayer.folder = QFileInfo(QUrl(newFolder).toLocalFile()).absolutePath();
    emit folderChanged();
}

const QString &HierarchyModel::getCountry() const
{
    return midlayer.country;
}

void HierarchyModel::setCountry(const QString &newCountry)
{
    if (midlayer.country == newCountry)
        return;
    midlayer.country = newCountry;
    emit countryChanged();
}

const QString &HierarchyModel::getProvince() const
{
    return midlayer.province;
}

void HierarchyModel::setProvince(const QString &newProvince)
{
    if (midlayer.province == newProvince)
        return;
    midlayer.province = newProvince;
    emit provinceChanged();
}

const QString &HierarchyModel::getCity() const
{
    return midlayer.city;
}

void HierarchyModel::setCity(const QString &newCity)
{
    if (midlayer.city == newCity)
        return;
    midlayer.city = newCity;
    emit cityChanged();
}

const QString &HierarchyModel::getOrganization() const
{
    return midlayer.organization;
}

void HierarchyModel::setOrganization(const QString &newOrganization)
{
    if (midlayer.organization == newOrganization)
        return;
    midlayer.organization = newOrganization;
    emit organizationChanged();
}

const QString &HierarchyModel::getCommon() const
{
    return midlayer.common;
}

void HierarchyModel::setCommon(const QString &newCommon)
{
    if (midlayer.common == newCommon)
        return;
    midlayer.common = newCommon;
    emit commonChanged();
}

const QString &HierarchyModel::getCypherSuite() const
{
    return midlayer.cypherSuite;
}

void HierarchyModel::setCypherSuite(const QString &newCypherSuite)
{
    if (midlayer.cypherSuite == newCypherSuite)
        return;
    midlayer.cypherSuite = newCypherSuite;
    emit cypherSuiteChanged();
}

const QString &HierarchyModel::getDaysValid() const
{
    return midlayer.daysValid;
}

void HierarchyModel::setDaysValid(const QString &newDaysValid)
{
    if (midlayer.daysValid == newDaysValid)
        return;
    midlayer.daysValid = newDaysValid;
    emit daysValidChanged();
}

const QString &HierarchyModel::getRootCAsuffix() const
{
    return midlayer.rootCAsuffix;
}

void HierarchyModel::setRootCAsuffix(const QString &newRootCAsuffix)
{
    if (midlayer.rootCAsuffix == newRootCAsuffix)
        return;
    midlayer.rootCAsuffix = newRootCAsuffix;
    emit rootCAsuffixChanged();
}

bool HierarchyModel::getThreelevels() const
{
    return midlayer.threelevels;
}

void HierarchyModel::setThreelevels(bool newThreelevels)
{
    if (midlayer.threelevels == newThreelevels)
        return;
    midlayer.threelevels = newThreelevels;
    emit threelevelsChanged();
}

const QString &HierarchyModel::getIntermediateCASuffix() const
{
    return midlayer.intermediateCASuffix;
}

void HierarchyModel::setIntermediateCASuffix(const QString &newIntermediateCASuffix)
{
    if (midlayer.intermediateCASuffix == newIntermediateCASuffix)
        return;
    midlayer.intermediateCASuffix = newIntermediateCASuffix;
    emit intermediateCASuffixChanged();
}
