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

    midlayer.settingsList.insert("Country","");
    midlayer.settingsList.insert("Province", "");
    midlayer.settingsList.insert("City", "");
    midlayer.settingsList.insert("Organization", "");
    midlayer.settingsList.insert("Common", "");
    midlayer.settingsList.insert("Root CA password", "");
    midlayer.settingsList.insert("Cypher Suite", "");
    midlayer.settingsList.insert("Days valid", "");
    midlayer.settingsList.insert("Root CA suffix", "");
    midlayer.settingsList.insert("Chain CA suffix", "");
    midlayer.settingsList.insert("Intermediate CA password", "");
    midlayer.settingsList.insert("Intermediate CA suffix", "");
}

int HierarchyModel::rowCount(const QModelIndex &) const
{
    return midlayer.settingsList.size();
}

QVariant HierarchyModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < rowCount())
        switch (role) {
        case NameRole: return midlayer.settingsList.key();
        case SettingRole: return midlayer.settingsList.at(index.row()).second;
        default: return QVariant();
    }
    return QVariant();
}

QHash<int, QByteArray> HierarchyModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { NameRole, "Name" },
        { SettingRole, "Setting" }
    };
    return roles;
}

QVariantMap HierarchyModel::get(int row) const
{
    QPair<QString, QString> setting = midlayer.settingsList.;
    return {{"Name", setting.first},
            {"Setting", setting.second}};
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

void HierarchyModel::set(int row, const QString &name, const QString &setting)
{
    if (row < 0 || row >= midlayer.settingsList.count())
        return;

    midlayer.settingsList.replace(row, {name, setting});
    emit dataChanged(index(row, 0), index(row, 0), {NameRole, SettingRole});
}

void HierarchyModel::save()
{
    QJsonObject settings;
    for (int index = 0; index < midlayer.settingsList.size(); index++) {
        settings.insert(midlayer.settingsList.at(index).first,
                        midlayer.settingsList.at(index).second);
    }

    QJsonDocument doc(settings);

    QString hierarchyPath = midlayer.hierarchyInfo.folder + "/" + "openssl_root_ca_config.json";
    QFile configFile(hierarchyPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        //ALARM
             return;
    configFile.write(doc.toJson());
    configFile.close();
}


void HierarchyModel::createNew()
{
    QDir().mkpath(midlayer.hierarchyInfo.folder);
    QDir().mkdir(midlayer.hierarchyInfo.folder);

    this->save();

    QDir().mkdir(midlayer.hierarchyInfo.folder + "/public/");
    QDir().mkdir(midlayer.hierarchyInfo.folder + "/private/");
    QDir().mkdir(midlayer.hierarchyInfo.folder + "/certs/");
    QDir().mkdir(midlayer.hierarchyInfo.folder + "/csr/");
}

void HierarchyModel::createRootAndIntermediate()
{
    midlayer.generateRootCertificate();
    midlayer.generateEndCertificate();
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
    for (QString key : settings.keys())  {
        midlayer.settingsList.append(QPair<QString,QString>(key, settings.value(key).toString()));
    }
    midlayer.hierarchyInfo.folder = QFileInfo(QUrl(file).toLocalFile()).absolutePath();
    endResetModel();
}
