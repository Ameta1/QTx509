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

    int generateECKey(EVP_PKEY **pkey);
    int generateRSAKey(EVP_PKEY **pkey, int numberOfBits);
    int saveEVPKey(EVP_PKEY **pkey, QString name);
    int generateCertReq(QString keyPath, QString reqPath);
    int createRootX509Cert(QString keyPath, QString certPath, int daysValid);
    int createIntermediateX509Cert(QString keyPath, QString certPath, int daysValid);
    int signCertReq(QString reqPath);
    bool isValidKeys(QString keyPath);
    bool isValidCertReq(QString reqPath);
private:
    int addInfo(X509_NAME *x509Name);
    const char		*szCountry = "RU";
    const char		*szProvince = "77";
    const char		*szCity = "Zelenograd";
    const char		*szOrganization = "MIET";
    const char		*szCommon = "localhost";
    int x509version = 3;
};

#endif // SSLMODEL_H
