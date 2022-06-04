#include "newmidlayer.h"
#include "ssldefs.h"
#include <QUrl>
#include <QDir>

newMidlayer::newMidlayer()
{
    std::initializer_list<QSettingPair> init {QSettingPair("folder", QDir::homePath()),
                                                QSettingPair("country", "RU"),
                                                QSettingPair("province","77"),
                                                QSettingPair("city","Zelenograd"),
                                                QSettingPair("organization","MIET"),
                                                QSettingPair("common","TCS"),
                                                QSettingPair("cypherSuite", cypherSuitesList.first()),
                                                QSettingPair("daysValid", 3650),
                                                QSettingPair("rootCAsuffix","root"),
                                                QSettingPair("threelevels",false),
                                                QSettingPair("intermediateCASuffix", "intermediate")};
    settingsList.append(init);
}

int newMidlayer::generateCertificate(QString identificator, QString ancestoCertFile, bool isRootCert)
{
    if (generateKeys(isRootCert, identificator)) {
        return Fail;
    }
    ancestoCertFile.chop(QString(".cert.pem").size());
    QString ancestorKeyFile = ancestoCertFile;
    if (sslFunctions::createLeafX509Cert(privateKeyFilename(ancestorKeyFile),
                                         publicKeyFilename(identificator),
                                         publicKeyFilename(ancestorKeyFile),
                                         intermediateCertFilename(identificator),
                                         packSslinfo())) {
        return Fail;
    }
    return OK;
}

int newMidlayer::generateRootCertificate()
{
    if (generateKeys(true, "Root")) {
        return Fail;
    }
    if (sslFunctions::createRootX509Cert(privateKeyFilename("Root"),
                                         rootCertFilename(),
                                         packSslinfo())) {
        return Fail;
    }
    return OK;
}

int newMidlayer::generateIntermediateCertificate(QString identificator)
{
    if (generateKeys(false, identificator)) {
        return Fail;
    }
    if (sslFunctions::createIntermediateX509Cert(privateKeyFilename("Root"),
                                                 publicKeyFilename(identificator),
                                                 intermediateCertFilename(identificator),
                                                 packSslinfo())) {
        return Fail;
    }
    return OK;
}

int newMidlayer::generateLeafCertificate(QString identificator, QString ancestoCertFile)
{
    if (generateKeys(false, identificator)) {
        return Fail;
    }
    ancestoCertFile.chop(QString(".cert.pem").size());
    QString ancestorKeyFile = ancestoCertFile;
    if (sslFunctions::createLeafX509Cert(privateKeyFilename(ancestorKeyFile),
                                         publicKeyFilename(identificator),
                                         publicKeyFilename(ancestorKeyFile),
                                         intermediateCertFilename(identificator),
                                         packSslinfo())) {
        return Fail;
    }
    return OK;
}

int newMidlayer::generateKeys(bool isRootCert, QString identificator)
{
    EVP_PKEY_ptr pkey (EVP_PKEY_new(), EVP_PKEY_free);
    EVP_PKEY *pkeyDP = pkey.get();
    switch (cypherSuitesList.indexOf(findSetting("cypherSuite"))) {
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
    if (sslFunctions::saveEVPPrivateKey(&pkeyDP, privateKeyFilename(identificator))) {
        return Fail;
    }
    if (isRootCert) {
        if (sslFunctions::saveEVPPublicKey(&pkeyDP, privateKeyFilename(identificator))) {
            return Fail;
        }
    }
    else {
        if (sslFunctions::saveEVPPublicKey(&pkeyDP, publicKeyFilename(identificator))) {
            return Fail;
        }
    }
    return OK;
}

QString newMidlayer::privateKeyFilename(QString identificator) const
{
    return findSetting("folder").toString() + "/private/"  + identificator +".key.pem";
}

QString newMidlayer::publicKeyFilename(QString identificator) const
{
    return findSetting("folder").toString() + "/public/" + identificator + ".key.pem";
}

QString newMidlayer::rootCertFilename() const
{
    return findSetting("folder").toString() + "/certs/" + findSetting("rootCAsuffix").toString() + ".cert.pem";
}

QString newMidlayer::intermediateCertFilename(QString identificator) const
{
    return findSetting("folder").toString() + "/certs/" + identificator + ".cert.pem";
}

QVariant newMidlayer::findSetting(QString name) const
{
    for (const auto& setting : settingsList) {
        if (setting.first == name) {
            return setting.second.toString();
        }
    }
    return "";
}

int newMidlayer::changeSetting(QString name, QString value)
{
    for (auto& setting : settingsList) {
        if (setting.first == name) {
            setting.second = value;
            return OK;
        }
    }
    return Fail;
}

int newMidlayer::size() const
{
    return settingsList.size();
}

Sslinfo newMidlayer::packSslinfo()
{
    Sslinfo sslinfo;
    sslinfo.szCountry = findSetting("country").toString();
    sslinfo.szProvince = findSetting("province").toString() ;
    sslinfo.szCity = findSetting("city").toString();
    sslinfo.szOrganization = findSetting("organization").toString();
    sslinfo.szCommon = findSetting("common").toString();
    sslinfo.daysValid = findSetting("daysValid").toInt();
    return sslinfo;
}
