#ifndef SSLFUNCTIONS_H
#define SSLFUNCTIONS_H

#include <QObject>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

//OSSL_LIB_CTX_load_config
//./Configure --debug no-autoload-config no-gost

#define x509version 3
#define rsabits 4096

typedef struct  {
    const unsigned char		*szCountry;
    const unsigned char		*szProvince;
    const unsigned char		*szCity;
    const unsigned char		*szOrganization;
    const unsigned char		*szCommon;
    const unsigned char     *rootCAPass;
} Sslinfo;

namespace sslFunctions {
    int generateECKey(EVP_PKEY **pkey);
    int generateRSAKey(EVP_PKEY **pkey, int numberOfBits);
    int generateGOSTaKey(EVP_PKEY **pkey);
    int generateGOSTbKey(EVP_PKEY **pkey);
    int generateGOSTcKey(EVP_PKEY **pkey);
    int saveEVPPrivateKey(EVP_PKEY **pkey, QString path);
    int saveEVPPublicKey(EVP_PKEY **pkey, QString path);
    int generateCertReq(QString keyPath, QString reqPath, Sslinfo sslinfo);
    int createIntermediateX509Cert(QString privateKeyPath, QString publicKeyPath, QString certPath, int daysValid, Sslinfo sslinfo);
    int createRootX509Cert(QString keyPath, QString certPath, int daysValid, Sslinfo sslinfo);
    int signCertReq(QString reqPath);
    bool isValidKeys(QString keyPath);
    bool isValidCertReq(QString reqPath);
    int addInfo(X509_NAME *x509Name, Sslinfo sslinfo);
};

#endif // SSLFUNCTIONS_H
