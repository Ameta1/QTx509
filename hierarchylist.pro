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
    hierarchymodel.h \
    midlayer.h \
    ssldefs.h \
    sslfunctions.h

SOURCES += \
    main.cpp \
    hierarchymodel.cpp \
    midlayer.cpp \
    sslfunctions.cpp

RESOURCES += \
    CustomTextField.qml \
    HierarchyDelegate.qml \
    HierarchyView.qml \
    HierarchyForm.qml \
    SectionDelegate.ui.qml \
    hierarchylist.qml
