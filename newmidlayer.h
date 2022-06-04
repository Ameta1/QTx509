#ifndef NEWMIDLAYER_H
#define NEWMIDLAYER_H

#include "sslfunctions.h"
#include "settingsmap.h"

enum cypherSuites {
    ECDSAsuite,
    RSAsuite,
    GOSTasuite,
    GOSTbsuite,
    GOSTcsuite
};

#define QSettingPair QPair<QString, QVariant>

class newMidlayer
{

public:
    explicit newMidlayer();
    const QStringList cypherSuitesList = {"ecdsa, prime256v1, sha256",
                                    "rsa, 4096, sha256",
                                    "gostr34102012_256a, GC256A, STRIBOG_256",
                                    "gostr34102012_256b, GC256B, STRIBOG_256",
                                    "gostr34102012_256c, GC256C, STRIBOG_256"};
    int generateRootCertificate();
    int generateIntermediateCertificate(QString identificator);
    int generateLeafCertificate(QString identificator, QString ancestoCertFile);
    QString rootCertFilename() const;

    QVariant findSetting(QString name) const;
    int changeSetting(QString name, QString value);
    int size() const;
    QList<QSettingPair> settingsList;
private:
    int generateCertificate(QString identificator, QString ancestoCertFile, bool isRootCert);
    int generateKeys(bool isRootCert, QString identificator);
    QString intermediateCertFilename(QString identificator) const;
    QString privateKeyFilename(QString identificator) const;
    QString publicKeyFilename(QString identificator) const;

    QString csrFilename();
    Sslinfo packSslinfo();
};

#endif // NEWMIDLAYER_H
