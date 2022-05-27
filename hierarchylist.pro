TEMPLATE = app
TARGET = hierarchylist
QT += quick quickcontrols2

INCLUDEPATH += \
$$PWD/../openssl/include \
$$PWD/../openssl/gost-engine/

DEPENDPATH += \
$$PWD/../openssl/include \
$$PWD/../openssl/gost-engine/

LIBS += \
-L$$PWD/../openssl/ -lcrypto \
-L$$PWD/../openssl/gost-engine/build/bin -lgost \

HEADERS += \
    certificatemodel.h \
    hierarchymodel.h \
    midlayer.h \
    settingsmap.h \
    ssldefs.h \
    sslfunctions.h

SOURCES += \
    certificatemodel.cpp \
    main.cpp \
    hierarchymodel.cpp \
    midlayer.cpp \
    settingsmap.cpp \
    sslfunctions.cpp

RESOURCES += \
    HierarchyView.qml \
    HierarchyForm.qml \
    hierarchylist.qml \
    IdentifierDialog.qml \
    SettingTextField.qml \
    Translation_en.ts

TRANSLATIONS = Translation_en.ts
DISTFILES += \

    SettingTextField.qml
