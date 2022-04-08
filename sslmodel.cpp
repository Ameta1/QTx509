#include "sslmodel.h"
#include "opensslAliases.h"
#include "exitCodes.h"
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
    if (ctx == NULL) {
        return Fail;
    }
    ret = EVP_PKEY_keygen_init(ctx.get());
    if (ret != 1) {
        qDebug() << ret;
        return Fail;
    }
    ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx.get(), NID_X9_62_prime256v1);
    if (ret != 1) {
        qDebug() << ret;
        return Fail;
    }
    ret = EVP_PKEY_keygen(ctx.get(), pkey);
    if (ret != 1) {
        qDebug() << ret;
        return Fail;
    }
    ret = 0;
    return ret;
}

// this takes a lot of time, consider launching it in a separate thread
int sslmodel::generateRSAKey(EVP_PKEY **pkey, int numberOfBits) {
    //TODO: Proper error handling
    BIGNUM_ptr publicKeyExponent (BN_new(), ::BN_free);
    BN_set_word(publicKeyExponent.get(), RSA_F4); // as recommended here https://www.openssl.org/docs/man1.1.1/man3/RSA_generate_key.html
    int ret = 1;
    RSA *rsa;

    rsa = RSA_new(); // no need to free, it will be freed when *pkey is freed

    ret = RSA_generate_key_ex(
                               rsa,
                               numberOfBits,
                               publicKeyExponent.get(),
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
    FILE_ptr file (fopen(path.toLocal8Bit(), "w"), ::fclose);
    if(!PEM_write_PKCS8PrivateKey(file.get(), *pkey, nullptr, nullptr, 0, nullptr, nullptr)) {
        qDebug() << "Handle me!!!";
        return 1;
    }
    if(!PEM_write_PUBKEY(file.get(), *pkey)) {
        qDebug() << "Handle me!!!";
        return 1;
    }
    return 0;
//    int ret = PEM_write_bio_PKCS8PrivateKey(file, pkey,
//                                                EVP_aes_256_cbc(),
//                                                passphrase,
//                                                passphrase.size(), NULL, NULL);
}

int sslmodel::addInfo(X509_NAME *x509Name) {
    int ret = 1;
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
    return 0;
}

int sslmodel::generateCertReq(QString keyPath, QString reqPath) {
    int ret = 1;

    FILE_ptr keyFile (fopen(keyPath.toLocal8Bit(), "r"), ::fclose);
    if (keyFile.get() == NULL) {
        return ret;
    }
    EVP_PKEY_ptr privateKey (PEM_read_PrivateKey(keyFile.get(),NULL, NULL, NULL), ::EVP_PKEY_free);
    if (privateKey.get() == NULL) {
        return ret;
    }
    EVP_PKEY_ptr publicKey (PEM_read_PUBKEY(keyFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
    if (publicKey.get() == NULL) {
        return ret;
    }

    X509_REQ_ptr x509Req (X509_REQ_new(), ::X509_REQ_free);
    ret = X509_REQ_set_version(x509Req.get(), x509version);
    if (ret != 1) {
        return ret;
    }

    X509_NAME *x509Name = X509_REQ_get_subject_name(x509Req.get()); // no need to free, will be freed as part of x509Req
    addInfo(x509Name);

    ret = X509_REQ_set_pubkey(x509Req.get(), publicKey.get());
    if (ret != 1) {
        return ret;
    }

    ret = X509_REQ_sign(x509Req.get(), privateKey.get(), EVP_sha256());	// return x509_req->signature->length
    if (ret <= 0) {
        return ret;
    }

    FILE_ptr certFile (fopen(reqPath.toLocal8Bit(), "w"), ::fclose);
    if (certFile.get() == NULL) {
        return ret;
    }
    ret = PEM_write_X509_REQ(certFile.get(), x509Req.get());
    if (ret <= 0) {
        return ret;
    }
    ret = 0;
    return ret;
}

int sslmodel::createRootX509Cert(QString keyPath, QString certPath, int daysValid) {
    int ret = 1;

    FILE_ptr keyFile (fopen(keyPath.toLocal8Bit(), "r"), ::fclose);
    if (keyFile.get() == NULL) {
        return ret;
    }
    EVP_PKEY_ptr privateKey (PEM_read_PrivateKey(keyFile.get(),NULL, NULL, NULL), ::EVP_PKEY_free);
    if (privateKey.get() == NULL) {
        return ret;
    }
    EVP_PKEY_ptr publicKey (PEM_read_PUBKEY(keyFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
    if (publicKey.get() == NULL) {
        return ret;
    }

    X509_ptr cert (X509_new(), ::X509_free);

    ret = X509_set_pubkey(cert.get(), publicKey.get());
    if (ret != 1) {
        return ret;
    }

    X509_NAME *x509Name = X509_get_subject_name(cert.get()); // no need to free, will be freed as part of x509Req
    ret = addInfo(x509Name);
    if (ret != 0) {
        return ret;
    }

    X509_gmtime_adj(X509_get_notBefore(cert.get()), 0);
    X509_gmtime_adj(X509_get_notAfter(cert.get()), daysValid * 24 * 3600);

    ret = X509_sign(cert.get(), privateKey.get(), EVP_sha256());
    if (ret == 0) {
        //HANDLE ME
        return ret;
    }
    ret = 1;

    FILE_ptr certFile (fopen(certPath.toLocal8Bit(), "w"), ::fclose);
    if (certFile.get() == NULL) {
        return ret;
    }
    ret = PEM_write_X509(certFile.get(), cert.get());
    if (ret == 0) {
        //HANDLE ME
        return ret;
    }
    ret = 0;
    return ret;
}

int sslmodel::createIntermediateX509Cert(QString keyPath, QString certPath, int daysValid) {
    int ret = 1;
    ret = 0;
    return ret;
}

bool sslmodel::isValidKeys(QString keyPath) {
    //consider moving this default values somewhere else
    size_t len = 100;
    char buf[100];

    FILE_ptr keyFile (fopen(keyPath.toLocal8Bit(), "r"), ::fclose);
    if (keyFile.get() == NULL) {
        return false;
    }
    EVP_PKEY_ptr privateKey (PEM_read_PrivateKey(keyFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
    if (privateKey.get() == NULL) {
        return false;
    }
    EVP_PKEY_ptr publicKey (PEM_read_PUBKEY(keyFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
    if (privateKey.get() == NULL) {
        return false;
    }
    unsigned long err = ERR_get_error();
    if (err != 0) {
        //there would be a signal to interface
        ERR_error_string_n(err, buf, len);
        return false;
    }
    int type = EVP_PKEY_type(privateKey.get()->type);
    switch (type)
    {
    case EVP_PKEY_RSA:
    case EVP_PKEY_RSA2: {
        RSA_ptr rsa (EVP_PKEY_get1_RSA(privateKey.get()), ::RSA_free);
        if (1 != RSA_check_key(rsa.get())) {
            //there would be a signal to interface
            err = ERR_get_error();
            ERR_error_string_n(err, buf, len);
            return false;
        }
        break;
    }
    case EVP_PKEY_EC: {
        EC_KEY_ptr ec (EVP_PKEY_get1_EC_KEY(privateKey.get()), ::EC_KEY_free);
        if (1 != EC_KEY_check_key(ec.get())) {
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
    FILE_ptr keyFile (fopen(reqPath.toLocal8Bit(), "r"), ::fclose);
    if (keyFile.get() == NULL) {
        return false;
    }
    X509_REQ_ptr request (PEM_read_X509_REQ(keyFile.get(), NULL, NULL, NULL), ::X509_REQ_free);
    unsigned long err = ERR_get_error();
    if (err != 0) {
        //there would be a signal to interface
        ERR_error_string_n(err, buf, len);
        return false;
    }
    return true;
}
