#include "certificatemodel.h"
#include <QDir>
certificateModel::certificateModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int certificateModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return certificateList.count();
}

QVariant certificateModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < rowCount())
        switch (role) {
        case StringRole: return certificateList.at(index.row()).first;
        case ForExportRole: return certificateList.at(index.row()).second;
        default: return QVariant();
    }
    return QVariant();
}

QHash<int, QByteArray> certificateModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { StringRole, "CertString" },
        { ForExportRole, "ForExport"}
    };
    return roles;
}

void certificateModel::reload(QString folder, QString filter = "*.*")
{
    QDir certDir(folder + "/certs/");
    QStringList certlist = certDir.entryList(QStringList(filter),QDir::Files);
    beginResetModel();
    certificateList.clear();
    for (auto certificate: certlist) {
        certificateList.append(QPair<QString, bool>(certificate, false));
    }
    endResetModel();
}

void certificateModel::checkForExport(const int index)
{
    certificateList[index].second = true;
}

void certificateModel::uncheckForExport(const int index)
{
    certificateList[index].second = false;
}

