#include "sslmodel.h"
#include "opensslAliases.h"
#include <QtDebug>

#include <openssl/x509.h>
#include <openssl/err.h>

sslmodel::sslmodel(QObject *parent)
    : QObject{parent}
{

}

int sslmodel::generateECKey(EVP_PKEY **pkey) {
    //TODO: Proper error handling
    EVP_PKEY_CTX_ptr ctx (EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL), ::EVP_PKEY_CTX_free);

    int ret;
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    handlePointers(ctx, check);
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

// this takes a lot of time, consider launching it in a separate thread
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

int sslmodel::generateCertReq(QString keyPath, QString reqPath) {
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
    if (NULL == (certFile = fopen(reqPath.toLocal8Bit(), "w"))) {
        fclose(keyFile);
        return ret;
    }
    ret = PEM_write_X509_REQ(certFile, x509Req);
    if (ret <= 0) {
        fclose(keyFile);
        return ret;
    }
    fclose(keyFile);
    ret = 0;
    return ret;
}

int sslmodel::signCertReq(QString reqPath) {
    FILE* keyFile = NULL;
    handlePointers(keyFile, fclose);
    if (NULL == (keyFile = fopen(reqPath.toLocal8Bit(), "r"))) {
        return false;
    }
    X509_REQ *request = PEM_read_X509_REQ(keyFile, NULL, NULL, NULL);
    handlePointers(request, X509_REQ_free);
}

bool sslmodel::isValidKeys(QString keyPath) {
    //consider moving this default values somewhere else
    size_t len = 100;
    char buf[100];
    FILE* keyFile;
    if (NULL == (keyFile = fopen(keyPath.toLocal8Bit(), "r"))) {
        fclose(keyFile);
        return false;
    }
    EVP_PKEY* pkey = PEM_read_PrivateKey(keyFile, NULL, NULL, NULL);
    pkey = PEM_read_PUBKEY(keyFile, NULL, NULL, NULL);
    fclose(keyFile);
    handlePointers(pkey, EVP_PKEY_free);
    unsigned long err = ERR_get_error();
    if (err != 0) {
        //there would be a signal to interface
        ERR_error_string_n(err, buf, len);
        return false;
    }
    int type = EVP_PKEY_type(pkey->type);
    switch (type)
    {
    case EVP_PKEY_RSA:
    case EVP_PKEY_RSA2: {
        RSA* rsa = EVP_PKEY_get1_RSA(pkey);
        handlePointers(rsa, RSA_free);
        if (1 != RSA_check_key(rsa)) {
            //there would be a signal to interface
            err = ERR_get_error();
            ERR_error_string_n(err, buf, len);
            return false;
        }
        break;
    }
    case EVP_PKEY_EC: {
        EC_KEY* ec = EVP_PKEY_get1_EC_KEY(pkey);
        handlePointers(ec, EC_KEY_free);
        if (1 != EC_KEY_check_key(ec)) {
            //there would be a signal to interface
            err = ERR_get_error();
            ERR_error_string_n(err, buf, len);
            return false;
        }
        break;
    }
    default:
        //there would be a signal to interface
        return false;
    }

    return true;
}

bool sslmodel::isValidCertReq(QString reqPath) {
    //consider moving this default values somewhere else
    size_t len = 100;
    char buf[100];
    FILE* keyFile;
    if (NULL == (keyFile = fopen(reqPath.toLocal8Bit(), "r"))) {
        fclose(keyFile);
        return false;
    }
    X509_REQ *request = PEM_read_X509_REQ(keyFile, NULL, NULL, NULL);
    fclose(keyFile);
    handlePointers(request, X509_REQ_free);
    unsigned long err = ERR_get_error();
    if (err != 0) {
        //there would be a signal to interface
        ERR_error_string_n(err, buf, len);
        return false;
    }
    return true;
}
