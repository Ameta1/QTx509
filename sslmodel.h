#ifndef SSLMODEL_H
#define SSLMODEL_H

#include <QObject>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

class sslmodel : public QObject
{
    Q_OBJECT
public:
    explicit sslmodel(QObject *parent = nullptr);

    int static generateECKey(EVP_PKEY **pkey);
    int static generateRSAKey(EVP_PKEY **pkey, int numberOfBits);
    int static saveEVPKey(EVP_PKEY **pkey, QString name);
    int static generateCertReq(QString keyPath, QString certPath);
};

#endif // SSLMODEL_H
