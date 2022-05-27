#ifndef MIDLAYER_H
#define MIDLAYER_H

#include <QObject>

#include "sslfunctions.h"
#include "settingsmap.h"

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
    const QStringList cypherSuitesList = {"ecdsa, prime256v1, sha256",
                                    "rsa, 4096, sha256",
                                    "gostr34102012_256a, GC256A, STRIBOG_256",
                                    "gostr34102012_256b, GC256B, STRIBOG_256",
                                    "gostr34102012_256c, GC256C, STRIBOG_256"};
    int generateRootCertificate();
    int generateIntermediateCertificate(QString identificator);
    int generateLeafCertificate(QString identificator, QString ancestoCertFile);
    QString rootCertFilename();

    QString folder;

    QString country;
    QString province;
    QString city;
    QString organization;
    QString common;

    QString rootCApassword;
    QString cypherSuite;
    QString daysValid;
    QString rootCAsuffix;
    QString chainCASuffix;

    bool threelevels;
    QString intermediateCAPassword;
    QString intermediateCASuffix;
private:
    int generateKeys(bool isRootCert, QString identificator);
    QString intermediateCertFilename(QString identificator);
    QString privateKeyFilename(QString identificator);
    QString publicKeyFilename(QString identificator);
    QString chainCertFilename();
    QString csrFilename();
    Sslinfo packSslinfo();
};

#endif // MIDLAYER_H
