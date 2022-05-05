#include <QtDebug>

#include <openssl/obj_mac.h>
#include <openssl/x509.h>
#include <openssl/err.h>

#include "sslfunctions.h"
#include "ssldefs.h"

namespace sslFunctions {

    int generateECKey(EVP_PKEY **pkey) {
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
        return OK;
    }

    // this takes a lot of time, consider launching it in a separate thread
    int generateRSAKey(EVP_PKEY **pkey, int numberOfBits) {
        //TODO: Proper error handling
        BIGNUM_ptr publicKeyExponent (BN_new(), ::BN_free);
        BN_set_word(publicKeyExponent.get(), RSA_F4); // as recommended here https://www.openssl.org/docs/man1.1.1/man3/RSA_generate_key.html
        int ret = 1;
        RSA *rsa;

        rsa = RSA_new(); // no need to free, it will be freed when *pkey is freed

        ret = RSA_generate_key_ex( rsa,
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

//    EVP_PKEY *create_key(const char *algname, const char *param)
//    {
//        EVP_PKEY *key1 = EVP_PKEY_new(), *newkey = NULL;
//        EVP_PKEY_CTX *ctx = NULL;

//        if (EVP_PKEY_set_type_str(key1, "gost2012_256", strlen("gost2012_256")) <= 0)
//              goto err;

//        if (!(ctx = EVP_PKEY_CTX_new(key1, NULL)))
//              goto err;

//        if (EVP_PKEY_keygen_init(ctx) == 0)
//              goto err;

//        if (ERR_peek_last_error())
//              goto err;

//        if (EVP_PKEY_CTX_ctrl_str(ctx, "paramset", param) <= 0)
//              goto err;

//        if (EVP_PKEY_keygen(ctx, &newkey) <= 0)

//              goto err;
    int generateGOSTaKey(EVP_PKEY **pkey)
    {
        if (EVP_PKEY_set_type(*pkey, NID_id_GostR3410_2012_256) <= 0) {
            qDebug() << EVP_PKEY_set_type(*pkey, NID_id_GostR3410_2012_256);
            return Fail;
        }

        EVP_PKEY_CTX_ptr ctx (EVP_PKEY_CTX_new_id(NID_id_GostR3410_2012_256, NULL), ::EVP_PKEY_CTX_free);

        int ret;
        if (ctx == NULL) {
            return Fail;
        }

        ret = EVP_PKEY_keygen_init(ctx.get());
        if (ret != 1) {
            qDebug() << ret;
            return Fail;
        }
        ret = EVP_PKEY_CTX_ctrl_str(ctx.get(), "paramset", "A");
        if (ret <= 0) {
            qDebug() << ret;
            return Fail;
        }
        ret = EVP_PKEY_keygen(ctx.get(), pkey);
        if (ret != 1) {
            qDebug() << ret;
            return Fail;
        }
        return OK;
    }

    //int Sslmodel::generateGOSTbKey(EVP_PKEY **pkey)
    //{

    //}

    //int Sslmodel::generateGOSTcKey(EVP_PKEY **pkey)
    //{

    //}

    //    int ret = PEM_write_bio_PKCS8PrivateKey(file, pkey,
    //                                                EVP_aes_256_cbc(),
    //                                                passphrase,
    //                                                passphrase.size(), NULL, NULL);
    int saveEVPPrivateKey(EVP_PKEY **pkey, QString path)
    {
        FILE_ptr file (fopen(path.toLocal8Bit(), "a"), ::fclose);
        if(!PEM_write_PKCS8PrivateKey(file.get(), *pkey, nullptr, nullptr, 0, nullptr, nullptr)) {
            qDebug() << "Handle me!!!";
            return Fail;
        }
        return OK;
    }

    int saveEVPPublicKey(EVP_PKEY **pkey, QString path)
    {
        FILE_ptr file (fopen(path.toLocal8Bit(), "a"), ::fclose);
        if(!PEM_write_PUBKEY(file.get(), *pkey)) {
            qDebug() << "Handle me!!!";
            return Fail;
        }
        return OK;
    }

    int addInfo(X509_NAME *x509Name, Sslinfo sslinfo) {
        int ret = X509_NAME_add_entry_by_txt(x509Name,"C", MBSTRING_ASC, sslinfo.szCountry, -1, -1, 0);
        if (ret != 1) {
            return Fail;
        }
        ret = X509_NAME_add_entry_by_txt(x509Name,"L", MBSTRING_ASC, sslinfo.szCity, -1, -1, 0);
        if (ret != 1) {
            return Fail;
        }
        ret = X509_NAME_add_entry_by_txt(x509Name,"ST", MBSTRING_ASC, sslinfo.szProvince, -1, -1, 0);
        if (ret != 1) {
            return Fail;
        }
        ret = X509_NAME_add_entry_by_txt(x509Name,"O", MBSTRING_ASC, sslinfo.szOrganization, -1, -1, 0);
        if (ret != 1) {
            return Fail;
        }
        ret = X509_NAME_add_entry_by_txt(x509Name,"CN", MBSTRING_ASC, sslinfo.szCommon, -1, -1, 0);
        if (ret != 1) {
            return Fail;
        }
        return OK;
    }

    //int ret = X509_NAME_add_entry_by_txt(x509Name,"C", MBSTRING_ASC,
    //                                     reinterpret_cast<const unsigned char *>(hierarchyInfo.szCountry.toLatin1().constData()), -1, -1, 0);
    //if (ret != 1) {
    //    return Fail;
    //}
    //ret = X509_NAME_add_entry_by_txt(x509Name,"L", MBSTRING_ASC,
    //                                 reinterpret_cast<const unsigned char *>(hierarchyInfo.szCity.toLatin1().constData()), -1, -1, 0);
    //if (ret != 1) {
    //    return Fail;
    //}
    //ret = X509_NAME_add_entry_by_txt(x509Name,"ST", MBSTRING_ASC,
    //                                 reinterpret_cast<const unsigned char *>(hierarchyInfo.szProvince.toLatin1().constData()), -1, -1, 0);
    //if (ret != 1) {
    //    return Fail;
    //}
    //ret = X509_NAME_add_entry_by_txt(x509Name,"O", MBSTRING_ASC,
    //                                 reinterpret_cast<const unsigned char *>(hierarchyInfo.szOrganization.toLatin1().constData()), -1, -1, 0);
    //if (ret != 1) {
    //    return Fail;
    //}
    //ret = X509_NAME_add_entry_by_txt(x509Name,"CN", MBSTRING_ASC,
    //                                 reinterpret_cast<const unsigned char *>(hierarchyInfo.szCommon.toLatin1().constData()), -1, -1, 0);
    int generateCertReq(QString keyPath, QString reqPath, Sslinfo sslinfo) {
        int ret = 1;

        FILE_ptr keyFile (fopen(keyPath.toLocal8Bit(), "r"), ::fclose);
        if (keyFile.get() == NULL) {
            return Fail;
        }
        EVP_PKEY_ptr privateKey (PEM_read_PrivateKey(keyFile.get(),NULL, NULL, NULL), ::EVP_PKEY_free);
        if (privateKey.get() == NULL) {
            return Fail;
        }
        EVP_PKEY_ptr publicKey (PEM_read_PUBKEY(keyFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
        if (publicKey.get() == NULL) {
            return Fail;
        }

        X509_REQ_ptr x509Req (X509_REQ_new(), ::X509_REQ_free);
        ret = X509_REQ_set_version(x509Req.get(), x509version);
        if (ret != 1) {
            return Fail;
        }

        X509_NAME *x509Name = X509_REQ_get_subject_name(x509Req.get()); // no need to free, will be freed as part of x509Req
        addInfo(x509Name, sslinfo);

        ret = X509_REQ_set_pubkey(x509Req.get(), publicKey.get());
        if (ret != 1) {
            return Fail;
        }

        ret = X509_REQ_sign(x509Req.get(), privateKey.get(), EVP_sha256());	// return x509_req->signature->length
        if (ret <= 0) {
            return Fail;
        }

        FILE_ptr certFile (fopen(reqPath.toLocal8Bit(), "w"), ::fclose);
        if (certFile.get() == NULL) {
            return Fail;
        }
        ret = PEM_write_X509_REQ(certFile.get(), x509Req.get());
        if (ret <= 0) {
            return Fail;
        }
        return Fail;
    }

    int createRootX509Cert(QString keyPath, QString certPath, int daysValid, Sslinfo sslinfo) {
        int ret = 1;

        FILE_ptr keyFile (fopen(keyPath.toLocal8Bit(), "r"), ::fclose);
        if (keyFile.get() == NULL) {
            return Fail;
        }
        EVP_PKEY_ptr privateKey (PEM_read_PrivateKey(keyFile.get(),NULL, NULL, NULL), ::EVP_PKEY_free);
        if (privateKey.get() == NULL) {
            return Fail;
        }
        EVP_PKEY_ptr publicKey (PEM_read_PUBKEY(keyFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
        if (publicKey.get() == NULL) {
            return Fail;
        }

        X509_ptr cert (X509_new(), ::X509_free);

        ret = X509_set_version(cert.get(), x509version);
        if (ret != 1) {
            return Fail;
        }

        ret = X509_set_pubkey(cert.get(), publicKey.get());
        if (ret != 1) {
            return Fail;
        }

        X509_NAME *x509Name = X509_get_subject_name(cert.get()); // no need to free, will be freed as part of x509
        ret = addInfo(x509Name, sslinfo);
        if (ret != 0) {
            return Fail;
        }

        X509_NAME *x509IssuerName = X509_get_issuer_name(cert.get()); // no need to free, will be freed as part of x509
        ret = addInfo(x509IssuerName, sslinfo);
        if (ret != 0) {
            return Fail;
        }

        X509_gmtime_adj(X509_get_notBefore(cert.get()), 0);
        X509_gmtime_adj(X509_get_notAfter(cert.get()), daysValid * 24 * 3600);

        ret = X509_sign(cert.get(), privateKey.get(), EVP_sha256());
        if (ret == 0) {
            //HANDLE ME
            return Fail;
        }

        FILE_ptr certFile (fopen(certPath.toLocal8Bit(), "w"), ::fclose);
        if (certFile.get() == NULL) {
            return Fail;
        }

        ret = PEM_write_X509(certFile.get(), cert.get());
        if (ret == 0) {
            //HANDLE ME
            return Fail;
        }

        return OK;
    }

    int createIntermediateX509Cert(QString privateKeyPath, QString publicKeyPath, QString certPath, int daysValid, Sslinfo sslinfo) {
        int ret = 1;

        FILE_ptr privateFile (fopen(privateKeyPath.toLocal8Bit(), "r"), ::fclose);
        if (privateFile.get() == NULL) {
            return Fail;
        }
        EVP_PKEY_ptr privateKey (PEM_read_PrivateKey(privateFile.get(),NULL, NULL, NULL), ::EVP_PKEY_free);
        if (privateKey.get() == NULL) {
            return Fail;
        }
        FILE_ptr publicFile (fopen(publicKeyPath.toLocal8Bit(), "r"), ::fclose);
        if (privateFile.get() == NULL) {
            return Fail;
        }
        EVP_PKEY_ptr publicKey (PEM_read_PUBKEY(publicFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
        if (publicKey.get() == NULL) {
            return Fail;
        }

        X509_ptr cert (X509_new(), ::X509_free);

        ret = X509_set_version(cert.get(), x509version);
        if (ret != 1) {
            return Fail;
        }

        ret = X509_set_pubkey(cert.get(), publicKey.get());
        if (ret != 1) {
            return Fail;
        }

        X509_NAME *x509Name = X509_get_subject_name(cert.get()); // no need to free, will be freed as part of x509Req
        ret = addInfo(x509Name, sslinfo);
        if (ret != 0) {
            return Fail;
        }

        X509_gmtime_adj(X509_get_notBefore(cert.get()), 0);
        X509_gmtime_adj(X509_get_notAfter(cert.get()), daysValid * 24 * 3600);

        ret = X509_sign(cert.get(), privateKey.get(), EVP_sha256());
        if (ret == 0) {
            //HANDLE ME
            return Fail;
        }

        FILE_ptr certFile (fopen(certPath.toLocal8Bit(), "w"), ::fclose);
        if (certFile.get() == NULL) {
            return Fail;
        }

        ret = PEM_write_X509(certFile.get(), cert.get());
        if (ret == 0) {
            //HANDLE ME
            return Fail;
        }

        return OK;
    }
    /*
    bool Sslmodel::isValidKeys(QString keyPath) {
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
        privateKey.get();
        int type = EVP_PKEY_type(privateKey.get() type);
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
    */
    bool isValidCertReq(QString reqPath) {
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
}

