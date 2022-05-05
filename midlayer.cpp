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
                                         hierarchyInfo.daysValid,
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
            break;
        }
        case GOSTbsuite: {
            break;
        }
        case GOSTcsuite: {
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
    sslinfo.szCountry = (unsigned char *)strdup(hierarchyInfo.szCountry.toLocal8Bit().constData());
    sslinfo.szProvince = (unsigned char *)strdup(hierarchyInfo.szProvince.toLocal8Bit().constData());
    sslinfo.szCity = (unsigned char *)strdup(hierarchyInfo.szCity.toLocal8Bit().constData());
    sslinfo.szOrganization = (unsigned char *)strdup(hierarchyInfo.szOrganization.toLocal8Bit().constData());
    sslinfo.szCommon = (unsigned char *)strdup(hierarchyInfo.szCommon.toLocal8Bit().constData());
    sslinfo.rootCAPass = (unsigned char *)strdup(hierarchyInfo.rootCApass.toLocal8Bit().constData());
//    memcpy(sslinfo.szCountry, hierarchyInfo.szCountry.toStdString().c_str() ,hierarchyInfo.szCountry.size());
//    memcpy(sslinfo.szProvince, hierarchyInfo.szProvince.toStdString().c_str() ,hierarchyInfo.szProvince.size());
//    memcpy(sslinfo.szCity, hierarchyInfo.szCity.toStdString().c_str() ,hierarchyInfo.szCity.size());
//    memcpy(sslinfo.szOrganization, hierarchyInfo.szOrganization.toStdString().c_str() ,hierarchyInfo.szOrganization.size());
//    memcpy(sslinfo.szCommon, hierarchyInfo.szCommon.toStdString().c_str() ,hierarchyInfo.szCommon.size());
//    memcpy(sslinfo.rootCAPass, hierarchyInfo.rootCApass.toStdString().c_str() ,hierarchyInfo.rootCApass.size());
    return sslinfo;
}
