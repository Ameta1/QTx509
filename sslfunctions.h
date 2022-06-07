#ifndef SSLFUNCTIONS_H
#define SSLFUNCTIONS_H

#include <QObject>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#define x509version 3
#define rsabits 4096

typedef struct  {
    QString szCountry;
    QString szProvince;
    QString szCity;
    QString szOrganization;
    QString szCommon;
    int daysValid;
} Sslinfo;

namespace sslFunctions {
    int generateECKey(EVP_PKEY **pkey);
    int generateRSAKey(EVP_PKEY **pkey, int numberOfBits);
    int generateGOSTKey(EVP_PKEY **pkey, const char *paramset);
    int saveEVPPrivateKey(EVP_PKEY **pkey, QString path);
    int saveEVPPublicKey(EVP_PKEY **pkey, QString path);
    int generateCertReq(QString keyFile, QString reqFile, Sslinfo sslinfo);
    int createRootX509Cert(QString keyFile, QString certFile, Sslinfo sslinfo);
    int createIntermediateX509Cert(QString privatekeyFile, QString publickeyFile, QString certFile, Sslinfo sslinfo);
    int createLeafX509Cert(QString privatekeyFile, QString publickeyFile, QString ancestorKeyfile, QString certFile, Sslinfo sslinfo);
    int signCertReq(QString reqPath);
    int addInfo(X509_NAME *x509Name, Sslinfo sslinfo);
    void sslError();
};

#endif // SSLFUNCTIONS_H
