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
            showError();
            return Fail;
        }
        ret = EVP_PKEY_keygen_init(ctx.get());
        if (ret != 1) {
            showError();
            return Fail;
        }
        ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx.get(), NID_X9_62_prime256v1);
        if (ret != 1) {
            showError();
            return Fail;
        }
        ret = EVP_PKEY_keygen(ctx.get(), pkey);
        if (ret != 1) {
            showError();
            return Fail;
        }
        return OK;
    }

    // this takes a lot of time
    int generateRSAKey(EVP_PKEY **pkey, int numberOfBits) {
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
            showError();
            return Fail;
        }

        EVP_PKEY_assign_RSA(*pkey, rsa);
        return OK;
    }

    int generateGOSTKey(EVP_PKEY **pkey, const char *paramset)
    {
        if (EVP_PKEY_set_type(*pkey, NID_id_GostR3410_2012_256) <= 0) {
            showError();
            return Fail;
        }

        EVP_PKEY_CTX_ptr ctx (EVP_PKEY_CTX_new_id(NID_id_GostR3410_2012_256, NULL), ::EVP_PKEY_CTX_free);

        int ret;
        if (ctx == NULL) {
            showError();
            return Fail;
        }

        ret = EVP_PKEY_keygen_init(ctx.get());
        if (ret != 1) {
            showError();
            return Fail;
        }
        ret = EVP_PKEY_CTX_ctrl_str(ctx.get(), "paramset", paramset);
        if (ret <= 0) {
            showError();
            return Fail;
        }
        ret = EVP_PKEY_keygen(ctx.get(), pkey);
        if (ret != 1) {
            showError();
            return Fail;
        }
        return OK;
    }

    //    int ret = PEM_write_bio_PKCS8PrivateKey(file, pkey,
    //                                                EVP_aes_256_cbc(),
    //                                                passphrase,
    //                                                passphrase.size(), NULL, NULL);
    int saveEVPPrivateKey(EVP_PKEY **pkey, QString path)
    {
        FILE_ptr file (fopen(path.toLocal8Bit(), "a"), ::fclose);
        if(!PEM_write_PKCS8PrivateKey(file.get(), *pkey, nullptr, nullptr, 0, nullptr, nullptr)) {
            showError();
            return Fail;
        }
        return OK;
    }

    int saveEVPPublicKey(EVP_PKEY **pkey, QString path)
    {
        FILE_ptr file (fopen(path.toLocal8Bit(), "a"), ::fclose);
        if(!PEM_write_PUBKEY(file.get(), *pkey)) {
            showError();
            return Fail;
        }
        return OK;
    }

    int addInfo(X509_NAME *x509Name, Sslinfo sslinfo) {
        int ret = X509_NAME_add_entry_by_txt(x509Name,"C", MBSTRING_ASC, (unsigned char*)sslinfo.szCountry.toLocal8Bit().constData(), -1, -1, 0);
        if (ret != 1) {
            showError();
            return Fail;
        }
        ret = X509_NAME_add_entry_by_txt(x509Name,"L", MBSTRING_ASC, (unsigned char*)sslinfo.szCity.toLocal8Bit().constData(), -1, -1, 0);
        if (ret != 1) {
            showError();
            return Fail;
        }
        ret = X509_NAME_add_entry_by_txt(x509Name,"ST", MBSTRING_ASC, (unsigned char*)sslinfo.szProvince.toLocal8Bit().constData(), -1, -1, 0);
        if (ret != 1) {
            showError();
            return Fail;
        }
        ret = X509_NAME_add_entry_by_txt(x509Name,"O", MBSTRING_ASC, (unsigned char*)sslinfo.szOrganization.toLocal8Bit().constData(), -1, -1, 0);
        if (ret != 1) {
            showError();
            return Fail;
        }
        ret = X509_NAME_add_entry_by_txt(x509Name,"CN", MBSTRING_ASC,(unsigned char*)sslinfo.szCommon.toLocal8Bit().constData(), -1, -1, 0);
        if (ret != 1) {
            showError();
            return Fail;
        }
        return OK;
    }

    int generateCertReq(QString keyFile, QString reqFile, Sslinfo sslinfo) {
        int ret = 1;

        FILE_ptr keyFileptr (fopen(keyFile.toLocal8Bit(), "r"), ::fclose);
        if (keyFileptr.get() == NULL) {
            showError();
            return Fail;
        }
        EVP_PKEY_ptr privateKey (PEM_read_PrivateKey(keyFileptr.get(),NULL, NULL, NULL), ::EVP_PKEY_free);
        if (privateKey.get() == NULL) {
            showError();
            return Fail;
        }
        EVP_PKEY_ptr publicKey (PEM_read_PUBKEY(keyFileptr.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
        if (publicKey.get() == NULL) {
            showError();
            return Fail;
        }

        X509_REQ_ptr x509Req (X509_REQ_new(), ::X509_REQ_free);
        ret = X509_REQ_set_version(x509Req.get(), x509version);
        if (ret != 1) {
            showError();
            return Fail;
        }

        X509_NAME *x509Name = X509_REQ_get_subject_name(x509Req.get()); // no need to free, will be freed as part of x509Req
        addInfo(x509Name, sslinfo);

        ret = X509_REQ_set_pubkey(x509Req.get(), publicKey.get());
        if (ret != 1) {
            showError();
            return Fail;
        }

        ret = X509_REQ_sign(x509Req.get(), privateKey.get(), EVP_sha256());	// return x509_req->signature->length
        if (ret <= 0) {
            showError();
            return Fail;
        }

        FILE_ptr certFile (fopen(reqFile.toLocal8Bit(), "w"), ::fclose);
        if (certFile.get() == NULL) {
            showError();
            return Fail;
        }
        ret = PEM_write_X509_REQ(certFile.get(), x509Req.get());
        if (ret <= 0) {
            showError();
            return Fail;
        }
        return OK;
    }

    int createRootX509Cert(QString keyFile, QString certFile, Sslinfo sslinfo) {
        int ret = 1;

        FILE_ptr keyFileptr (fopen(keyFile.toLocal8Bit(), "r"), ::fclose);
        if (keyFileptr.get() == NULL) {
            showError();
            return Fail;
        }
        EVP_PKEY_ptr privateKey (PEM_read_PrivateKey(keyFileptr.get(),NULL, NULL, NULL), ::EVP_PKEY_free);
        if (privateKey.get() == NULL) {
            showError();
            return Fail;
        }
        EVP_PKEY_ptr publicKey (PEM_read_PUBKEY(keyFileptr.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
        if (publicKey.get() == NULL) {
            showError();
            return Fail;
        }

        X509_ptr cert (X509_new(), ::X509_free);

        ret = X509_set_version(cert.get(), x509version);
        if (ret != 1) {
            showError();
            return Fail;
        }

        ret = X509_set_pubkey(cert.get(), publicKey.get());
        if (ret != 1) {
            showError();
            return Fail;
        }

        X509_NAME *x509Name = X509_get_subject_name(cert.get()); // no need to free, will be freed as part of x509
        ret = addInfo(x509Name, sslinfo);
        if (ret != 0) {
            showError();
            return Fail;
        }

        X509_NAME *x509IssuerName = X509_get_issuer_name(cert.get()); // no need to free, will be freed as part of x509
        ret = addInfo(x509IssuerName, sslinfo);
        if (ret != 0) {
            showError();
            return Fail;
        }

        X509_gmtime_adj(X509_get_notBefore(cert.get()), 0);
        X509_gmtime_adj(X509_get_notAfter(cert.get()), sslinfo.daysValid * 24 * 3600);

        if (EVP_PKEY_get_id(privateKey.get()) == NID_id_GostR3410_2012_256) {
            ret = X509_sign(cert.get(), privateKey.get(), EVP_get_digestbynid(NID_id_GostR3410_2012_256));
        }
        else {
            ret = X509_sign(cert.get(), privateKey.get(), EVP_sha256());
        }
        if (ret == 0) {
            showError();
            return Fail;
        }
        ret = X509_verify(cert.get(), publicKey.get());
        if (ret != 1) {
            showError();
            return Fail;
        }

        FILE_ptr certFileptr (fopen(certFile.toLocal8Bit(), "w"), ::fclose);
        if (certFileptr.get() == NULL) {
            showError();
            return Fail;
        }

        ret = PEM_write_X509(certFileptr.get(), cert.get());
        if (ret == 0) {
            showError();
            return Fail;
        }

        return OK;
    }

    int createIntermediateX509Cert(QString privatekeyFile, QString publickeyFile, QString certFile, Sslinfo sslinfo) {
        FILE_ptr privateFile (fopen(privatekeyFile.toLocal8Bit(), "r"), ::fclose);
        if (privateFile.get() == NULL) {
            showError();
            return Fail;
        }
        EVP_PKEY_ptr privateKey (PEM_read_PrivateKey(privateFile.get(),NULL, NULL, NULL), ::EVP_PKEY_free);
        if (privateKey.get() == NULL) {
            showError();
            return Fail;
        }
        FILE_ptr publicFile (fopen(publickeyFile.toLocal8Bit(), "r"), ::fclose);
        if (privateFile.get() == NULL) {
            showError();
            return Fail;
        }
        EVP_PKEY_ptr publicKey (PEM_read_PUBKEY(publicFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
        if (publicKey.get() == NULL) {
            showError();
            return Fail;
        }
        EVP_PKEY_ptr rootPublicKey (PEM_read_PUBKEY(privateFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free); //for verification
        if (publicKey.get() == NULL) {
            showError();
            return Fail;
        }

        X509_ptr cert (X509_new(), ::X509_free);

        int ret = X509_set_version(cert.get(), x509version);
        if (ret != 1) {
            showError();
            return Fail;
        }

        ret = X509_set_pubkey(cert.get(), publicKey.get());
        if (ret != 1) {
            showError();
            return Fail;
        }

        X509_NAME *x509Name = X509_get_subject_name(cert.get()); // no need to free, will be freed as part of x509Req
        ret = addInfo(x509Name, sslinfo);
        if (ret != 0) {
            showError();
            return Fail;
        }

        X509_gmtime_adj(X509_get_notBefore(cert.get()), 0);
        X509_gmtime_adj(X509_get_notAfter(cert.get()), sslinfo.daysValid * 24 * 3600);

        if (EVP_PKEY_get_id(privateKey.get()) == NID_id_GostR3410_2012_256) {
            ret = X509_sign(cert.get(), privateKey.get(), EVP_get_digestbynid(NID_id_GostR3410_2012_256));
        }
        else {
            ret = X509_sign(cert.get(), privateKey.get(), EVP_sha256());
        }
        if (ret == 0) {
            showError();
            return Fail;
        }

        ret = X509_verify(cert.get(), rootPublicKey.get());
        if (ret != 1) {
            showError();
            return Fail;
        }

        FILE_ptr certFileptr (fopen(certFile.toLocal8Bit(), "w"), ::fclose);
        if (certFileptr.get() == NULL) {
            showError();
            return Fail;
        }

        ret = PEM_write_X509(certFileptr.get(), cert.get());
        if (ret == 0) {
            showError();
            return Fail;
        }

        return OK;
    }

    int createLeafX509Cert(QString privatekeyFile, QString publickeyFile, QString ancestorKeyfile, QString certFile, Sslinfo sslinfo)
    {
        FILE_ptr privateFile (fopen(privatekeyFile.toLocal8Bit(), "r"), ::fclose);
        if (privateFile.get() == NULL) {
            showError();
            return Fail;
        }
        EVP_PKEY_ptr privateKey (PEM_read_PrivateKey(privateFile.get(),NULL, NULL, NULL), ::EVP_PKEY_free);
        if (privateKey.get() == NULL) {
            showError();
            return Fail;
        }
        FILE_ptr publicFile (fopen(publickeyFile.toLocal8Bit(), "r"), ::fclose);
        if (privateFile.get() == NULL) {
            showError();
            return Fail;
        }
        EVP_PKEY_ptr publicKey (PEM_read_PUBKEY(publicFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free);
        if (publicKey.get() == NULL) {
            showError();
            return Fail;
        }
        FILE_ptr ancestorFile (fopen(ancestorKeyfile.toLocal8Bit(), "r"), ::fclose);
        if (ancestorFile.get() == NULL) {
            showError();
            return Fail;
        }
        EVP_PKEY_ptr ancestorKey (PEM_read_PUBKEY(ancestorFile.get(), NULL, NULL, NULL), ::EVP_PKEY_free); //for verification
        if (publicKey.get() == NULL) {
            showError();
            return Fail;
        }

        X509_ptr cert (X509_new(), ::X509_free);

        int ret = X509_set_version(cert.get(), x509version);
        if (ret != 1) {
            showError();
            return Fail;
        }

        ret = X509_set_pubkey(cert.get(), publicKey.get());
        if (ret != 1) {
            showError();
            return Fail;
        }

        X509_NAME *x509Name = X509_get_subject_name(cert.get()); // no need to free, will be freed as part of x509Req
        ret = addInfo(x509Name, sslinfo);
        if (ret != 0) {
            showError();
            return Fail;
        }

        X509_gmtime_adj(X509_get_notBefore(cert.get()), 0);
        X509_gmtime_adj(X509_get_notAfter(cert.get()), sslinfo.daysValid * 24 * 3600);

        if (EVP_PKEY_get_id(privateKey.get()) == NID_id_GostR3410_2012_256) {
            ret = X509_sign(cert.get(), privateKey.get(), EVP_get_digestbynid(NID_id_GostR3410_2012_256));
        }
        else {
            ret = X509_sign(cert.get(), privateKey.get(), EVP_sha256());
        }
        if (ret == 0) {
            showError();
            return Fail;
        }

        ret = X509_verify(cert.get(), ancestorKey.get());
        if (ret != 1) {
            showError();
            return Fail;
        }

        FILE_ptr certFileptr (fopen(certFile.toLocal8Bit(), "w"), ::fclose);
        if (certFileptr.get() == NULL) {
            showError();
            return Fail;
        }

        ret = PEM_write_X509(certFileptr.get(), cert.get());
        if (ret == 0) {
            showError();
            return Fail;
        }

        return OK;
    }

    void showError()
    {
        char buf[100];
        unsigned long err = ERR_get_error();
        ERR_error_string_n(err, buf, sizeof(buf));
        qDebug() << buf;
    }

}

