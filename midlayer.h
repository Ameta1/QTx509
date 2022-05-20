#ifndef MIDLAYER_H
#define MIDLAYER_H

#include <QObject>
#include <QMap>
#include "sslfunctions.h"
typedef struct {
    QString szCountry; //"RU"
    QString szProvince; //"77"
    QString szCity; //"Zelenograd"
    QString szOrganization; //"MIET"
    QString szCommon; //"localhost"
    QString rootCApass; //1234
    QString cypherSuite;
    int daysValid; //3650
    QString caChainSuffix;
    QString caRootSuffix;
    QString folder;
} HierarchyInfo;

enum cypherSuites {
    ECDSAsuite,
    RSAsuite,
    GOSTasuite,
    GOSTbsuite,
    GOSTcsuite
};

class Midlayer : public QObject
{
    Q_OBJECT
public:
    explicit Midlayer(QObject *parent = nullptr);
    HierarchyInfo hierarchyInfo;
    const QStringList cypherSuitesList = {"ecdsa, prime256v1, sha256",
                                    "rsa, 4096, sha256",
                                    "gostr34102012_256a, GC256A, STRIBOG_256",
                                    "gostr34102012_256b, GC256B, STRIBOG_256",
                                    "gostr34102012_256c, GC256C, STRIBOG_256"};
    int generateRootCertificate();
    int generateIntermediateCertificate();
    int generateEndCertificate();
    QMap<QString, QString> settingsList;
private:
    int generateKeys(bool isRootCert, QString identificator);
    QString privateKeysPath(QString identificator);
    QString publicKeysPath(QString identificator);
    QString rootCertPath();
    QString chainCertPath();
    QString csrPath();
    Sslinfo packSslinfo();
};

#endif // MIDLAYER_H
