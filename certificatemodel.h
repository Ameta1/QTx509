#ifndef CERTIFICATEMODEL_H
#define CERTIFICATEMODEL_H

#include <QAbstractListModel>
#include "pkcs11/Common.h"

class certificateModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CertificateRole {
        StringRole = Qt::DisplayRole,
        ForExportRole
    };
    Q_ENUM(CertificateRole)
    explicit certificateModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

public slots:
    void reload(QString folder, QString filter);

    void checkForExport(const int index);
    void uncheckForExport(const int index);

    int exportToSmartCard();
private:
    QList<QVariantMap> certificateList;
};

#endif // CERTIFICATEMODEL_H
