#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLocale>
#include <QTranslator>
#include <sslmodel.h>
#include <opensslAliases.h>
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "qSshMaster_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/qSshMaster/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    EVP_PKEY_ptr pkey (EVP_PKEY_new(), EVP_PKEY_free);
    EVP_PKEY *pkeyDP = pkey.get();
    qDebug() <<"generateECKey"<< sslmodel::generateECKey(&pkeyDP);
    qDebug() <<"saveEVPKey"<< sslmodel::saveEVPKey(&pkeyDP, "/home/gregory/MY_COOL_KEY.pem");
    qDebug() <<"generateCertReq"<< sslmodel::generateCertReq("/home/gregory/MY_COOL_KEY.pem", "/home/gregory/MY_COOL_CERTIFICATE_REQUEST.pem");
    qDebug() <<"isValidKeys"<< sslmodel::isValidKeys("/home/gregory/MY_COOL_KEY.pem");

    return app.exec();
}
