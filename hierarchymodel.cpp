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

HierarchyModel::HierarchyModel(QObject *parent ) : QAbstractListModel(parent)
{
    midlayer.hierarchyInfo = { };
}

int HierarchyModel::rowCount(const QModelIndex &) const
{
    return 1;
}
enum HierarchyRole {
    CountryRole = Qt::DisplayRole,
    ProvinceRole,
    CityRole,
    OrgRole,
    CommonRole,
    rootPassRole,
    cypherSuiteRole,
    daysValidRole,
    chainSuffixRole,
    rootSuffixRole,
    folderRole
};

QVariant HierarchyModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < rowCount())
        switch (role) {
        case CountryRole: return midlayer.hierarchyInfo.szCountry;
        case ProvinceRole: return midlayer.hierarchyInfo.szProvince;
        case CityRole: return midlayer.hierarchyInfo.szCity;
        case OrgRole: return midlayer.hierarchyInfo.szOrganization;
        case CommonRole: return midlayer.hierarchyInfo.szCommon;
        case rootPassRole: return midlayer.hierarchyInfo.rootCApass;
        case cypherSuiteRole: return midlayer.hierarchyInfo.cypherSuite;
        case daysValidRole: return midlayer.hierarchyInfo.daysValid;
        case chainSuffixRole: return midlayer.hierarchyInfo.caChainSuffix;
        case rootSuffixRole: return midlayer.hierarchyInfo.caRootSuffix;
        case folderRole: return midlayer.hierarchyInfo.folder;
        default: return QVariant();
    }
    return QVariant();
}

QHash<int, QByteArray> HierarchyModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { CountryRole, "szCountry" },
        { ProvinceRole, "szProvince" },
        { CityRole, "szCity" },
        { OrgRole, "szOrganization" },
        { CommonRole, "szCommon" },
        { rootPassRole, "rootCApass" },
        { cypherSuiteRole, "cypherSuite" },
        { daysValidRole, "daysValid" },
        { chainSuffixRole, "caChainSuffix" },
        { rootSuffixRole, "caRootSuffix" },
    };
    return roles;
}

QVariantMap HierarchyModel::get() const
{
    return {{"szCountry", midlayer.hierarchyInfo.szCountry},
            {"szProvince", midlayer.hierarchyInfo.szProvince},
            {"szCity", midlayer.hierarchyInfo.szCity},
            {"szOrganization", midlayer.hierarchyInfo.szOrganization},
            {"szCommon", midlayer.hierarchyInfo.szCommon},
            {"rootPass", midlayer.hierarchyInfo.rootCApass},
            {"cypherSuite", midlayer.hierarchyInfo.cypherSuite},
            {"daysValid", midlayer.hierarchyInfo.daysValid},
            {"chainSuffix", midlayer.hierarchyInfo.caChainSuffix},
            {"rootSuffix", midlayer.hierarchyInfo.caRootSuffix},
    };
}

QString HierarchyModel::folder()
{
    return midlayer.hierarchyInfo.folder;
}

void HierarchyModel::setFolder(QString folder)
{
    midlayer.hierarchyInfo.folder = folder;
    emit folderChanged();
}

void HierarchyModel::set(const QString &szCountry,
                       const QString &szProvince,
                       const QString &szCity,
                       const QString &szOrganization,
                       const QString &szCommon,
                       const QString &rootCApass,
                       const QString &cypherSuite,
                       int daysValid,
                       const QString &caChainSuffix,
                       const QString &caRootSuffix,
                       const QString &folder)
{
    midlayer.hierarchyInfo = HierarchyInfo{
            szCountry,
            szProvince,
            szCity,
            szOrganization,
            szCommon,
            rootCApass,
            cypherSuite,
            daysValid,
            caChainSuffix,
            caRootSuffix,
            folder};
    emit dataChanged(index(0, 0), index(0, 0), {CountryRole,
                                                ProvinceRole,
                                                CityRole,
                                                OrgRole,
                                                CommonRole,
                                                rootPassRole,
                                                cypherSuiteRole,
                                                daysValidRole,
                                                chainSuffixRole,
                                                rootSuffixRole,
                                                folderRole});
}



void HierarchyModel::save()
{
    QJsonObject settings;
    settings.insert("szCountry", midlayer.hierarchyInfo.szCountry);
    settings.insert("szProvince", midlayer.hierarchyInfo.szProvince);
    settings.insert("szCity", midlayer.hierarchyInfo.szCity);
    settings.insert("szOrganization", midlayer.hierarchyInfo.szOrganization);
    settings.insert("szCommon", midlayer.hierarchyInfo.szCommon);
    settings.insert("rootPass", midlayer.hierarchyInfo.rootCApass);
    settings.insert("cypherSuite", midlayer.hierarchyInfo.cypherSuite);
    settings.insert("daysValid", midlayer.hierarchyInfo.daysValid);
    settings.insert("chainSuffix", midlayer.hierarchyInfo.caChainSuffix);
    settings.insert("rootSuffix", midlayer.hierarchyInfo.caRootSuffix);

    QJsonDocument doc(settings);

    QString hierarchyPath(QUrl(midlayer.hierarchyInfo.folder).toLocalFile() + "/" + "openssl_root_ca_config.json");
    QFile configFile(hierarchyPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
             return;
    configFile.write(doc.toJson());
    configFile.close();
}


void HierarchyModel::createNew()
{
    QString localfilepath(QUrl(midlayer.hierarchyInfo.folder).toLocalFile());
    QDir().mkpath(localfilepath);
    QDir().mkdir(localfilepath);
    this->save();
    QDir().mkdir(localfilepath + "/public/");
    QDir().mkdir(localfilepath + "/private/");
    QDir().mkdir(localfilepath + "/certs/");
    QDir().mkdir(localfilepath + "/csr/");
}

void HierarchyModel::createRootAndIntermediate()
{
    midlayer.generateRootCertificate();
    midlayer.generateEndCertificate();
}

QStringList HierarchyModel::cypherSuites()
{
    return midlayer.cypherSuitesList;
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

    beginResetModel();
    midlayer.hierarchyInfo.szCountry = settings.value("szCountry").toString();
    midlayer.hierarchyInfo.szProvince = settings.value("szProvince").toString();
    midlayer.hierarchyInfo.szCity = settings.value("szCity").toString();
    midlayer.hierarchyInfo.szOrganization = settings.value("szOrganization").toString();
    midlayer.hierarchyInfo.szCommon = settings.value("szCommon").toString();
    midlayer.hierarchyInfo.rootCApass = settings.value("rootPass").toString();
    midlayer.hierarchyInfo.cypherSuite = settings.value("cypherSuite").toString();
    midlayer.hierarchyInfo.daysValid = settings.value("daysValid").toInt();
    midlayer.hierarchyInfo.caChainSuffix = settings.value("chainSuffix").toString();
    midlayer.hierarchyInfo.caRootSuffix = settings.value("rootSuffix").toString();
    midlayer.hierarchyInfo.folder = QFileInfo(QUrl(file).toLocalFile()).absolutePath();;
    qDebug() <<midlayer.hierarchyInfo.folder;
    endResetModel();
}
