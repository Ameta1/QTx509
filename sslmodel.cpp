#include "sslmodel.h"
#include <QtDebug>

#include <openssl/x509.h>
sslmodel::sslmodel(QObject *parent)
    : QObject{parent}
{

}

template<typename T, typename D>
std::unique_ptr<T, D> handlePointers(T* pointer, D deleter)
{
    return std::unique_ptr<T, D>{pointer, deleter};
}

int sslmodel::generateECKey(EVP_PKEY **pkey) {
    //TODO: Proper error handling
    EVP_PKEY_CTX *ctx = NULL;
    handlePointers(ctx, EVP_PKEY_CTX_free);
    int ret;
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);

    if (ctx == NULL) {
        return -1;
    }
    ret = EVP_PKEY_keygen_init(ctx);
    if (ret != 1) {
        qDebug() << ret;
        return ret;
    }
    ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1);
    if (ret != 1) {
        qDebug() << ret;
        return ret;
    }
    ret = EVP_PKEY_keygen(ctx, pkey);
    if (ret != 1) {
        qDebug() << ret;
        return ret;
    }
    ret = 0;
    return ret;
}

int sslmodel::generateRSAKey(EVP_PKEY **pkey, int numberOfBits) {
    //TODO: Proper error handling
    BIGNUM *publicKeyExponent = NULL;
    handlePointers(publicKeyExponent, BN_free);
    publicKeyExponent = BN_new();
    BN_set_word(publicKeyExponent, RSA_F4); // as recommended here https://www.openssl.org/docs/man1.1.1/man3/RSA_generate_key.html
    int ret = 1;
    RSA *rsa;

    rsa = RSA_new(); // no need to free, it will be freed when *pkey is freed

    ret = RSA_generate_key_ex( // documentaion says i need to seed random, internet says otherwise, need to check
                               rsa,
                               numberOfBits,
                               publicKeyExponent,
                               NULL);
    if (ret != 1) {
        qDebug() << ret;
        return ret;
    }

    EVP_PKEY_assign_RSA(*pkey, rsa);
    ret = 0;
    return ret;
}


int sslmodel::saveEVPKey(EVP_PKEY **pkey, QString path) {
    //TODO: Proper error handling, check if file already exists
    FILE *file;
    file = fopen(path.toLocal8Bit(), "w");
    if(!PEM_write_PKCS8PrivateKey(file, *pkey, nullptr, nullptr, 0, nullptr, nullptr)) {
        qDebug() << "Handle me!!!";
        fclose(file);
        return 1;
    }
    if(!PEM_write_PUBKEY(file, *pkey)) {
        qDebug() << "Handle me!!!";
        fclose(file);
        return 1;
    }
    fclose(file);
    return 0;
//    int ret = PEM_write_bio_PKCS8PrivateKey(file, pkey,
//                                                EVP_aes_256_cbc(),
//                                                passphrase,
//                                                passphrase.size(), NULL, NULL);
}

int sslmodel::generateCertReq(QString keyPath, QString certPath) {
    FILE *keyFile = NULL;
    FILE *certFile = NULL;
    //TODO: make them class members
    const char		*szCountry = "RU";
    const char		*szProvince = "77";
    const char		*szCity = "Zelenograd";
    const char		*szOrganization = "MIET";
    const char		*szCommon = "localhost";

    X509_REQ *x509Req = NULL;
    handlePointers(x509Req, X509_REQ_free);
    X509_NAME *x509Name = NULL;
    handlePointers(x509Name, X509_NAME_free);
    int x509version = 1;

    // Let's made these keys separate, to avoid confusion,
    // despite they can be stored in a single structure
    EVP_PKEY *privateKey = NULL;
    EVP_PKEY *publicKey = NULL;

    handlePointers(privateKey, EVP_PKEY_free);
    handlePointers(publicKey, EVP_PKEY_free);
    int ret = 1;
    if (NULL == (keyFile = fopen(keyPath.toLocal8Bit(), "r"))) {
        fclose(keyFile);
        return ret;
    }

    if (NULL == (privateKey = PEM_read_PrivateKey(keyFile,NULL, NULL, NULL))) {
        fclose(keyFile);
        return ret;
    }
    if (NULL == (publicKey = PEM_read_PUBKEY(keyFile, NULL, NULL, NULL))) {
        fclose(keyFile);
        return ret;
    }

    fclose(keyFile);

    x509Req = X509_REQ_new();
    ret = X509_REQ_set_version(x509Req, x509version);
    if (ret != 1) {
        return ret;
    }

    x509Name = X509_REQ_get_subject_name(x509Req);

    ret = X509_NAME_add_entry_by_txt(x509Name,"C", MBSTRING_ASC, (const unsigned char*)szCountry, -1, -1, 0);
    if (ret != 1) {
        return ret;
    }
    ret = X509_NAME_add_entry_by_txt(x509Name,"L", MBSTRING_ASC, (const unsigned char*)szCity, -1, -1, 0);
    if (ret != 1) {
        return ret;
    }
    ret = X509_NAME_add_entry_by_txt(x509Name,"ST", MBSTRING_ASC, (const unsigned char*)szProvince, -1, -1, 0);
    if (ret != 1) {
        return ret;
    }
    ret = X509_NAME_add_entry_by_txt(x509Name,"O", MBSTRING_ASC, (const unsigned char*)szOrganization, -1, -1, 0);
    if (ret != 1) {
        return ret;
    }
    ret = X509_NAME_add_entry_by_txt(x509Name,"CN", MBSTRING_ASC, (const unsigned char*)szCommon, -1, -1, 0);
    if (ret != 1) {
        return ret;
    }
//    X509_gmtime_adj(X509_get_notBefore(),0);
//    X509_gmtime_adj(X509_get_notAfter(),(long)60*60*24*days);
    ret = X509_REQ_set_pubkey(x509Req, publicKey);
    if (ret != 1) {
        return ret;
    }

    ret = X509_REQ_sign(x509Req, privateKey, EVP_sha256());	// return x509_req->signature->length
    if (ret <= 0) {
        return ret;
    }
    if (NULL == (certFile = fopen(certPath.toLocal8Bit(), "w"))) {
        fclose(keyFile);
        return ret;
    }
    ret = PEM_write_X509_REQ(certFile, x509Req);
    if (ret <= 0) {
        fclose(keyFile);
        return ret;
    }
    fclose(keyFile);
    return 0;
}
