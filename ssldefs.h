#ifndef SSLDEFS_H
#define SSLDEFS_H

#include <openssl/ssl.h>
#include <openssl/engine.h>
#include <memory>

using EC_KEY_ptr = std::unique_ptr<EC_KEY, decltype(&::EC_KEY_free)>;

using ENGINE_ptr = std::unique_ptr<ENGINE, decltype(&::ENGINE_free)>;
using BIGNUM_ptr = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;

using RSA_ptr = std::unique_ptr<RSA, decltype(&::RSA_free)>;

using EVP_PKEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
using EVP_PKEY_CTX_ptr = std::unique_ptr<EVP_PKEY_CTX, decltype(&::EVP_PKEY_CTX_free)>;

using BN_ptr = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;

using FILE_ptr = std::unique_ptr<FILE, decltype(&::fclose)>;

using BIO_MEM_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;
using BIO_FILE_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;

using X509_ptr = std::unique_ptr<X509, decltype(&::X509_free)>;
using X509_REQ_ptr = std::unique_ptr<X509_REQ, decltype(&::X509_REQ_free)>;
using X509_EXTENSION_ptr = std::unique_ptr<X509_EXTENSION, decltype(&::X509_EXTENSION_free)>;

using ASN1_INTEGER_ptr = std::unique_ptr<ASN1_INTEGER, decltype(&::ASN1_INTEGER_free)>;
using ASN1_TIME_ptr = std::unique_ptr<ASN1_TIME, decltype(&::ASN1_TIME_free)>;

using X509_NAME_ptr = std::unique_ptr<X509_NAME, decltype(&::X509_NAME_free)>;
using X509_NAME_ENTRY_ptr = std::unique_ptr<X509_NAME_ENTRY, decltype(&::X509_NAME_ENTRY_free)>;

using X509_STORE_ptr = std::unique_ptr<X509_STORE, decltype(&::X509_STORE_free)>;
using X509_LOOKUP_ptr = std::unique_ptr<X509_LOOKUP, decltype(&::X509_LOOKUP_free)>;
using X509_STORE_CTX_ptr = std::unique_ptr<X509_STORE_CTX, decltype(&::X509_STORE_CTX_free)>;

enum exitCodes {
    OK      = 0,
    Fail    = 1
};

#endif // SSLDEFS_H
