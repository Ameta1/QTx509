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
    if (sslFunctions::createRootX509Cert(this->privateKeysPath("Root"),
                                         this->rootCertPath(),
                                         settingsList.value("Days valid"),
                                         this->packSslinfo())) {
        return Fail;
    }
    return OK;
}
int Midlayer::generateEndCertificate()
{
    if (generateKeys(false, "End")) {
        return Fail;
    }
    if (sslFunctions::createIntermediateX509Cert(this->privateKeysPath("Root"),
                                                 this->publicKeysPath("End"),
                                                 this->chainCertPath(),
                                                 hierarchyInfo.daysValid,
                                                 this->packSslinfo())) {
        return Fail;
    }
    return OK;
}

int Midlayer::generateKeys(bool isRootCert, QString identificator)
{
    EVP_PKEY_ptr pkey (EVP_PKEY_new(), EVP_PKEY_free);
    EVP_PKEY *pkeyDP = pkey.get();
    switch (cypherSuitesList.indexOf(hierarchyInfo.cypherSuite)) {
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
    if (sslFunctions::saveEVPPrivateKey(&pkeyDP, this->privateKeysPath(identificator))) {
        return Fail;
    }
    if (isRootCert) {
        if (sslFunctions::saveEVPPublicKey(&pkeyDP, this->privateKeysPath(identificator))) {
            return Fail;
        }
    }
    else {
        if (sslFunctions::saveEVPPublicKey(&pkeyDP, this->publicKeysPath(identificator))) {
            return Fail;
        }
    }
    return OK;
}

QString Midlayer::privateKeysPath(QString identificator)
{
    return hierarchyInfo.folder + "/private/" + hierarchyInfo.szOrganization + identificator +".key.pem";
}

QString Midlayer::publicKeysPath(QString identificator)
{
    return hierarchyInfo.folder + "/public/" + hierarchyInfo.szOrganization + identificator + ".key.pem";
}

QString Midlayer::rootCertPath()
{
    return hierarchyInfo.folder + "/certs/" + hierarchyInfo.caRootSuffix + ".cert.pem";
}

QString Midlayer::chainCertPath()
{
    return hierarchyInfo.folder + "/certs/" + hierarchyInfo.caChainSuffix + ".cert.pem";
}

QString Midlayer::csrPath()
{
    return hierarchyInfo.folder + "/csr/" + hierarchyInfo.szOrganization + "csr.pem";
}

Sslinfo Midlayer::packSslinfo()
{
    Sslinfo sslinfo;
    sslinfo.szCountry = hierarchyInfo.szCountry;
    sslinfo.szProvince = hierarchyInfo.szProvince;
    sslinfo.szCity = hierarchyInfo.szCity;
    sslinfo.szOrganization = hierarchyInfo.szOrganization;
    sslinfo.szCommon = hierarchyInfo.szCommon;
    sslinfo.rootCAPass = hierarchyInfo.rootCApass;
    return sslinfo;
}
