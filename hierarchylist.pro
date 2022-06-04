TEMPLATE = app
TARGET = hierarchylist
QT += quick quickcontrols2

INCLUDEPATH += \
$$PWD/../openssl/include \
$$PWD/../openssl/gost-engine/ \
$$PWD/pkcs11/

DEPENDPATH += \
$$PWD/../openssl/include \
$$PWD/../openssl/gost-engine/ \
$$PWD/pkcs11/

LIBS += \
-L$$PWD/../openssl/ -lcrypto \
-L$$PWD/../openssl/gost-engine/build/bin -lgost \
-L$$PWD -lrtpkcs11ecp \
-L$$PWD -lpki-core \
-ldl

HEADERS += \
    certificatemodel.h \
    hierarchymodel.h \
    midlayer.h \
    newhierarchymodel.h \
    newmidlayer.h \
    settingscontainer.h \
    settingsmap.h \
    ssldefs.h \
    sslfunctions.h

SOURCES += \
    certificatemodel.cpp \
    main.cpp \
    hierarchymodel.cpp \
    midlayer.cpp \
    newhierarchymodel.cpp \
    newmidlayer.cpp \
    settingscontainer.cpp \
    settingsmap.cpp \
    sslfunctions.cpp

RESOURCES += \
    HierarchyView.qml \
    HierarchyForm.qml \
    hierarchylist.qml \
    IdentifierDialog.qml \
    SettingTextField.qml \
    newMain.qml
    Translation_en.ts

TRANSLATIONS = Translation_en.ts

DISTFILES += \
    newMain.qml
