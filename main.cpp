#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

extern "C"
{
   #include<gost-engine.h>
}

#include "hierarchymodel.h"
#include "ssldefs.h"
#include "certificatemodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ENGINE_load_gost();
    ENGINE_ptr engine (ENGINE_by_id("gost"), ENGINE_free);
    if(!engine.get())
    {
        return Fail; //Failed to get structural reference to engine
    }
    ENGINE_set_default(engine.get(), ENGINE_METHOD_ALL);

    QQuickStyle::setStyle("Material");
    qmlRegisterType<CertificateModel>("Custom.CertificateModel", 1, 0, "CertificateModel");
    QScopedPointer<HierarchyModel> hierarchyModel(new HierarchyModel);
    QQmlApplicationEngine qmlApplicationEngine;
    qmlRegisterSingletonInstance("Custom.HierarchyModel", 1, 0, "HierarchyModel", hierarchyModel.get());
    qmlApplicationEngine.load(QUrl(QStringLiteral("qrc:/hierarchylist.qml")));

    return app.exec();
}

