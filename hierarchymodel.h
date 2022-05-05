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

#ifndef HIERARCHYMODEL_H
#define HIERARCHYMODEL_H

#include <QAbstractListModel>
#include "midlayer.h"

class HierarchyModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString folder READ folder WRITE setFolder NOTIFY folderChanged)
public:
    enum HierarchyRole {
        CountryRole = Qt::DisplayRole,
        ProvinceRole = Qt::UserRole,
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
    Q_ENUM(HierarchyRole)

    HierarchyModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex & = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE QVariantMap get() const;
    QString folder();
    void setFolder(QString folder);
signals:
    void folderChanged();
public slots:
    void set(const QString &szCountry,
             const QString &szProvince,
             const QString &szCity,
             const QString &szOrganization,
             const QString &szCommon,
             const QString &rootCApass,
             const QString &cypherSuite,
             int daysValid,
             const QString &caChainSuffix,
             const QString &caRootSuffix,
             const QString &folder
             );
    //change void to smth to handle errors
    void save();
    void load(QString file);
    void createNew();
    void createRootAndIntermediate();
    QStringList cypherSuites();
private:
    Midlayer midlayer;
};

#endif // HIERARCHYMODEL_H
