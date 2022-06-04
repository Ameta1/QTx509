#ifndef NEWHIERARCHYMODEL_H
#define NEWHIERARCHYMODEL_H

#include <QAbstractListModel>
#include "newmidlayer.h"
#include "settingscontainer.h"

class newHierarchyModel : public QAbstractListModel
{
    Q_OBJECT

//    Q_PROPERTY(bool threelevels READ getThreelevels WRITE setThreelevels NOTIFY threelevelsChanged)
    Q_PROPERTY(bool rootCreated READ isRootCreated NOTIFY rootCreatedChanged)
public:
    explicit newHierarchyModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    enum CertificateRole {
        NameRole = Qt::DisplayRole,
        ValueRole
    };
    Q_ENUM(CertificateRole)

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool isRootCreated();
//    bool getThreelevels() const;
//    void setThreelevels(bool newThreelevels);


public slots:
    //change void to smth to handle errors
    void newData(settingsContainer azaza);
    void save();
    void load(QString file);
    void createNew();
    void createRoot();
    void createRootAndIntermediate();
    void createAdditionalIntermediate(QString identificator);
    void createLeaf(QString identificator, QString ancestorFile);

signals:
    void openErrorPopup();
    void threelevelsChanged();
    void certListChanged();
    void folderChanged();

    void rootCreatedChanged();

private:
    newMidlayer midlayer;
};

#endif // NEWHIERARCHYMODEL_H
