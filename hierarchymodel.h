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
