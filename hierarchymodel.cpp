/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Hierarchy: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the hierarchy form at https://www.qt.io/hierarchy-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
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
    settings.insert("rootCApassword", midlayer.rootCApassword);
    settings.insert("cypherSuite", midlayer.cypherSuite);
    settings.insert("daysValid", midlayer.daysValid);
    settings.insert("rootCAsuffix", midlayer.rootCAsuffix);
    settings.insert("chainCASuffix", midlayer.chainCASuffix);
    if (midlayer.threelevels) {
        settings.insert("intermediateCAPassword", midlayer.intermediateCAPassword);
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
    midlayer.generateRootCertificate();
}

void HierarchyModel::createRootAndIntermediate()
{
    midlayer.generateRootCertificate();
    midlayer.generateIntermediateCertificate(midlayer.intermediateCASuffix);
}

void HierarchyModel::createAdditionalIntermediate(QString identificator)
{
    midlayer.generateIntermediateCertificate(identificator);
}

void HierarchyModel::createLeaf(QString identificator, QString ancestorFile)
{
    midlayer.generateLeafCertificate(identificator, ancestorFile);
}

bool HierarchyModel::rootCreated()
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
    midlayer.rootCApassword = settings.value("rootCApassword").toString();
    emit rootCApasswordChanged();
    midlayer.cypherSuite = settings.value("cypherSuite").toString();
    emit cypherSuiteChanged();
    midlayer.daysValid = settings.value("daysValid").toString();
    emit daysValidChanged();
    midlayer.rootCAsuffix = settings.value("rootCAsuffix").toString();
    emit rootCAsuffixChanged();
    midlayer.chainCASuffix = settings.value("chainCASuffix").toString();
    emit chainCASuffixChanged();

    if (!(settings.value("intermediateCAPassword").isUndefined()) &&
            (!settings.value("intermediateCASuffix").isUndefined())) {
        midlayer.intermediateCAPassword = settings.value("intermediateCAPassword").toString();
        emit intermediateCAPasswordChanged();
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

const QString &HierarchyModel::getRootCApassword() const
{
    return midlayer.rootCApassword;
}

void HierarchyModel::setRootCApassword(const QString &newRootCApassword)
{
    if (midlayer.rootCApassword == newRootCApassword)
        return;
    midlayer.rootCApassword = newRootCApassword;
    emit rootCApasswordChanged();
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

const QString &HierarchyModel::getChainCASuffix() const
{
    return midlayer.chainCASuffix;
}

void HierarchyModel::setChainCASuffix(const QString &newChainCASuffix)
{
    if (midlayer.chainCASuffix == newChainCASuffix)
        return;
    midlayer.chainCASuffix = newChainCASuffix;
    emit chainCASuffixChanged();
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

const QString &HierarchyModel::getIntermediateCAPassword() const
{
    return midlayer.intermediateCAPassword;
}

void HierarchyModel::setIntermediateCAPassword(const QString &newIntermediateCAPassword)
{
    if (midlayer.intermediateCAPassword == newIntermediateCAPassword)
        return;
    midlayer.intermediateCAPassword = newIntermediateCAPassword;
    emit intermediateCAPasswordChanged();
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
