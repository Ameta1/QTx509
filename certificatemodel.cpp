#include "certificatemodel.h"
#include <QDir>
#include <iostream>
#include "pkcs11/common.h"
#include "openssl/x509.h"

CertificateModel::CertificateModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int CertificateModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return certificateList.count();
}

QVariant CertificateModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < rowCount())
        switch (role) {
        case StringRole: return certificateList.at(index.row()).value("CertString");
        case ForExportRole: return certificateList.at(index.row()).value("ForExport");
        default: return QVariant();
    }
    return QVariant();
}

QHash<int, QByteArray> CertificateModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { StringRole, "CertString" },
        { ForExportRole, "ForExport"}
    };
    return roles;
}

void CertificateModel::reload(QString folder, QString filter)
{
    QDir certDir(folder + "/certs/");
    QStringList certStringList = certDir.entryList(QStringList(filter),QDir::Files);
    beginResetModel();
    certificateList.clear();
    for (const QString& certString: certStringList) {
        QVariantMap certInfo;
        certInfo.insert("CertString", certString);
        certInfo.insert("ForExport", false);
        certInfo.insert("FullPath", certDir.path() + "/" + certString);
        certificateList.append(certInfo);
    }
    endResetModel();
}

void CertificateModel::checkForExport(const int index)
{
    certificateList[index]["ForExport"] = true;
}

void CertificateModel::uncheckForExport(const int index)
{
    certificateList[index]["ForExport"] = false;
}

int CertificateModel::exportToSmartCard()
{
    try {
            rutoken::pkicore::initialize(std::string("/usr/lib/")); //NOTE IN BUILDING PROCESS!!
            SCOPE_EXIT() {
                rutoken::pkicore::deinitialize();
            };

            auto devices = rutoken::pkicore::Pkcs11Device::enumerate();
            if (devices.empty()) {
                throw std::runtime_error("There must be at least one device connected");
            }

            auto device = std::move(devices.front());

            device.login("12345678");
            SCOPE_EXIT(&device) {
                device.logout();
            };
            for (const auto& certificate : certificateList) {
                qDebug() << "Reading certificate from " << certificate.value("FullPath").toString();
                auto certData = readFile(certificate.value("FullPath").toString().toStdString());
                device.importCert(rutoken::pkicore::ExternalCert(std::string(certData.begin(), certData.end())), false);
            }
            auto certs = device.enumerateCerts();
            qDebug() << certs.size() <<" certificates now stored on SC";

        } catch (const std::exception& e) {
            qDebug() << e.what();
            return 1;
        }
    return 0;
}

int CertificateModel::deleteAllCertsOnSC()
{
    try {
            rutoken::pkicore::initialize("/home/gregory/diplom/qSshMaster");
            SCOPE_EXIT() {
                rutoken::pkicore::deinitialize();
            };

            auto devices = rutoken::pkicore::Pkcs11Device::enumerate();
            if (devices.empty()) {
                throw std::runtime_error("There must be at least one device connected");
            }

            auto device = std::move(devices.front());

            device.login("12345678");
            SCOPE_EXIT(&device) {
                device.logout();
            };
            auto certs = device.enumerateCerts();
            for (const auto& cert : certs) {
                device.deleteCert(cert);
            }
            certs = device.enumerateCerts();
            qDebug() << certs.size() <<" certificates now stored on SC";

        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    return 0;
}

