#include "midlayer.h"
#include "ssldefs.h"
#include <QUrl>
Midlayer::Midlayer(QObject *parent)
    : QObject{parent}
{
}

int Midlayer::generateRootCertificate()
{
    if (generateKeys(true, "Root")) {
        return Fail;
    }
    if (sslFunctions::createRootX509Cert(this->privateKeyFilename("Root"),
                                         this->rootCertFilename(),
                                         this->packSslinfo())) {
        return Fail;
    }
    return OK;
}

int Midlayer::generateIntermediateCertificate(QString identificator)
{
    if (generateKeys(false, identificator)) {
        return Fail;
    }
    if (sslFunctions::createIntermediateX509Cert(this->privateKeyFilename("Root"),
                                                 this->publicKeyFilename(identificator),
                                                 this->intermediateCertFilename(identificator),
                                                 this->packSslinfo())) {
        return Fail;
    }
    return OK;
}

int Midlayer::generateLeafCertificate(QString identificator, QString ancestoCertFile)
{
    if (generateKeys(false, identificator)) {
        return Fail;
    }
    ancestoCertFile.chop(QString(".cert.pem").size());
    QString ancestorKeyFile = ancestoCertFile;
    if (sslFunctions::createLeafX509Cert(this->privateKeyFilename(ancestorKeyFile),
                                         this->publicKeyFilename(identificator),
                                         this->publicKeyFilename(ancestorKeyFile),
                                         this->intermediateCertFilename(identificator),
                                         this->packSslinfo())) {
        return Fail;
    }
    return OK;
}

int Midlayer::generateKeys(bool isRootCert, QString identificator)
{
    EVP_PKEY_ptr pkey (EVP_PKEY_new(), EVP_PKEY_free);
    EVP_PKEY *pkeyDP = pkey.get();
    switch (cypherSuitesList.indexOf(cypherSuite)) {
        case ECDSAsuite: {
            if (sslFunctions::generateECKey(&pkeyDP)) {
                return Fail;
            }
            break;
        }
        case RSAsuite: {
            if (sslFunctions::generateRSAKey(&pkeyDP, rsabits)) {
                return Fail;
            }
            break;
        }
        case GOSTasuite: {
            if (sslFunctions::generateGOSTKey(&pkeyDP, "A")) {
                return Fail;
            }
            break;
        }
        case GOSTbsuite: {
            if (sslFunctions::generateGOSTKey(&pkeyDP, "B")) {
                return Fail;
            }
            break;
        }
        case GOSTcsuite: {
            if (sslFunctions::generateGOSTKey(&pkeyDP, "C")) {
                return Fail;
            }
            break;
        }
        default: {
            return Fail;
        }
    }
    if (sslFunctions::saveEVPPrivateKey(&pkeyDP, this->privateKeyFilename(identificator))) {
        return Fail;
    }
    if (isRootCert) {
        if (sslFunctions::saveEVPPublicKey(&pkeyDP, this->privateKeyFilename(identificator))) {
            return Fail;
        }
    }
    else {
        if (sslFunctions::saveEVPPublicKey(&pkeyDP, this->publicKeyFilename(identificator))) {
            return Fail;
        }
    }
    return OK;
}

QString Midlayer::privateKeyFilename(QString identificator)
{
    return folder + "/private/"  + identificator +".key.pem";
}

QString Midlayer::publicKeyFilename(QString identificator)
{
    return folder + "/public/" + identificator + ".key.pem";
}

QString Midlayer::rootCertFilename()
{
    return folder + "/certs/" + rootCAsuffix + ".cert.pem";
}

QString Midlayer::intermediateCertFilename(QString identificator)
{
    return folder + "/certs/" + identificator + ".cert.pem";
}

QString Midlayer::chainCertFilename()
{
    return folder + "/certs/" + chainCASuffix + ".cert.pem";
}

QString Midlayer::csrFilename()
{
    return folder + "/csr/" + organization + "csr.pem";
}

Sslinfo Midlayer::packSslinfo()
{
    Sslinfo sslinfo;
    sslinfo.szCountry = country;
    sslinfo.szProvince = province ;
    sslinfo.szCity = city;
    sslinfo.szOrganization = organization;
    sslinfo.szCommon = common;
    sslinfo.daysValid = daysValid.toInt();
    return sslinfo;
}
