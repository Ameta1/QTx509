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

class HierarchyModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString folder READ getFolder WRITE setFolder NOTIFY folderChanged)
    Q_PROPERTY(QString country READ getCountry WRITE setCountry NOTIFY countryChanged)
    Q_PROPERTY(QString province READ getProvince WRITE setProvince NOTIFY provinceChanged)
    Q_PROPERTY(QString city READ getCity WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString organization READ getOrganization WRITE setOrganization NOTIFY organizationChanged)
    Q_PROPERTY(QString common READ getCommon WRITE setCommon NOTIFY commonChanged)
    Q_PROPERTY(QString cypherSuite READ getCypherSuite WRITE setCypherSuite NOTIFY cypherSuiteChanged)
    Q_PROPERTY(QString daysValid READ getDaysValid WRITE setDaysValid NOTIFY daysValidChanged)
    Q_PROPERTY(QString rootCAsuffix READ getRootCAsuffix WRITE setRootCAsuffix NOTIFY rootCAsuffixChanged)

    Q_PROPERTY(QString intermediateCASuffix READ getIntermediateCASuffix WRITE setIntermediateCASuffix NOTIFY intermediateCASuffixChanged)

    Q_PROPERTY(bool threelevels READ getThreelevels WRITE setThreelevels NOTIFY threelevelsChanged)
    Q_PROPERTY(bool rootCreated READ isRootCreated NOTIFY rootCreatedChanged)

public:
    HierarchyModel(QObject *parent = nullptr);

    const QString &getFolder() const;
    void setFolder(const QString &newFolder);
    const QString &getCountry() const;
    void setCountry(const QString &newCountry);
    const QString &getProvince() const;
    void setProvince(const QString &newProvince);
    const QString &getCity() const;
    void setCity(const QString &newCity);
    const QString &getOrganization() const;
    void setOrganization(const QString &newOrganization);
    const QString &getCommon() const;
    void setCommon(const QString &newCommon);
    const QString &getCypherSuite() const;
    void setCypherSuite(const QString &newCypherSuite);
    const QString &getDaysValid() const;
    void setDaysValid(const QString &newDaysValid);
    const QString &getRootCAsuffix() const;
    void setRootCAsuffix(const QString &newRootCAsuffix);

    const QString &getIntermediateCASuffix() const;
    void setIntermediateCASuffix(const QString &newIntermediateCASuffix);

    bool isRootCreated();
    bool getThreelevels() const;
    void setThreelevels(bool newThreelevels);


public slots:
    //change void to smth to handle errors
    void save();
    void load(QString file);
    void createNew();
    void createRoot();
    void createRootAndIntermediate();
    void createAdditionalIntermediate(QString identificator);
    void createLeaf(QString identificator, QString ancestorFile);

signals:
    void countryChanged();
    void provinceChanged();
    void cityChanged();
    void organizationChanged();
    void commonChanged();
    void cypherSuiteChanged();
    void daysValidChanged();
    void rootCAsuffixChanged();

    void intermediateCASuffixChanged();

    void threelevelsChanged();
    void certListChanged();
    void folderChanged();

    void rootCreatedChanged();

private:
    Midlayer midlayer;
};

#endif // HIERARCHYMODEL_H
