TEMPLATE = app
TARGET = hierarchylist
QT += quick quickcontrols2

INCLUDEPATH += \
$$PWD/../openssl/ \
$$PWD/../openssl/gost-engine/

DEPENDPATH += \
$$PWD/../openssl/ \
$$PWD/../openssl/gost-engine/

LIBS += \
-L/home/gregory/diplom/openssl/ -lcrypto \
-L/home/gregory/diplom/openssl/gost-engine/build/bin -lgost \

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
