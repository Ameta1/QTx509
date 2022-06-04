#ifndef SETTINGSCONTAINER_H
#define SETTINGSCONTAINER_H

#include <QObject>

class settingsContainer
{
    Q_GADGET
public:
    explicit settingsContainer();
    virtual ~settingsContainer() {};
    QString folder;

    QString country;
    QString province;
    QString city;
    QString organization;
    QString common;

    QString cypherSuite;
    QString daysValid;
    QString rootCAsuffix;

    bool threelevels;
    QString intermediateCASuffix;

};

#endif // SETTINGSCONTAINER_H
