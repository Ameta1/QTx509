/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Hierarchy: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the hierarchy form at https://www.qt.io/hierarchy-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

extern "C"
{
   #include<gost-engine.h>
}

#include "hierarchymodel.h"
#include <openssl/obj_mac.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/engine.h>
#include "ssldefs.h"
#include "sslfunctions.h"
#include "certificatemodel.h"

void debug() {
    Sslinfo sslinfo {
        "RU",
        "77",
        "zelenograd",
        "miet",
        "localhost",
        3650
    };
    EVP_PKEY_ptr pkey (EVP_PKEY_new(), EVP_PKEY_free);
    EVP_PKEY *pkeyDP = pkey.get();
    qDebug() <<"generateGOSTKey"<< sslFunctions::generateGOSTKey(&pkeyDP, "A");
    qDebug() <<"saveEVPKey"<< sslFunctions::saveEVPPrivateKey(&pkeyDP, "/home/gregory/MY_COOL_KEY.pem");
    qDebug() <<"saveEVPKey"<< sslFunctions::saveEVPPublicKey(&pkeyDP, "/home/gregory/MY_COOL_KEY.pem");
    qDebug() << "createRootCert" << sslFunctions::createRootX509Cert("/home/gregory/MY_COOL_KEY.pem", "/home/gregory/MY_COOL_CERT.crt", sslinfo);
};

//CK_ATTRIBUTE dataObjectTemplate[] =
//{
//    { CKA_CLASS, &dataObject, sizeof(dataObject) },                       // Класс - бинарные данные
//    { CKA_APPLICATION, applicationLabel, sizeof(applicationLabel) - 1 },  // Описание приложения, которое записывает данные
//    { CKA_LABEL, dataObjectLabel, sizeof(dataObjectLabel) - 1 }           // Метка объекта с данными
//};

//int deleteshit() {
//    HMODULE module;                                   // Хэндл загруженной библиотеки PKCS#11
//    CK_SESSION_HANDLE session;                        // Хэндл открытой сессии

//    CK_FUNCTION_LIST_PTR functionList;                // Указатель на список функций PKCS#11, хранящийся в структуре CK_FUNCTION_LIST
//    CK_C_GetFunctionList getFunctionList;             // Указатель на функцию C_GetFunctionList

//    CK_SLOT_ID_PTR slotsSC;                             // Массив идентификаторов слотов
//    CK_ULONG slotCount;                               // Количество идентификаторов слотов в массиве

//    CK_OBJECT_HANDLE_PTR objects;                     // Массив найденных объектов
//    CK_ULONG objectCount;                             // Количество хэндлов объектов в массиве

//    CK_RV rv;                                         // Код возврата. Могут быть возвращены только ошибки, определенные в PKCS#11
//    int r;                                            // Код возврата для функций, возвращающих int

//    CK_ULONG i;                                       // Вспомогательная переменная-счетчик в циклах

//    int errorCode = 1;                                // Флаг ошибки

//    /*************************************************************************
//    * Выполнить действия для начала работы с библиотекой PKCS#11             *
//    *************************************************************************/
//    printf("Initialization...\n");

//    /*************************************************************************
//    * Загрузить библиотеку                                                   *
//    *************************************************************************/
//    module = LoadLibrary(PKCS11ECP_LIBRARY_NAME);
//    CHECK(" LoadLibrary", module != NULL, exit);

//    /*************************************************************************
//    * Получить адрес функции запроса структуры с указателями на функции      *
//    *************************************************************************/
//    getFunctionList = (CK_C_GetFunctionList)GetProcAddress(module, "C_GetFunctionList");
//    CHECK(" GetProcAddress", getFunctionList != NULL, unload_pkcs11);

//    /*************************************************************************
//    * Получить структуру с указателями на функции                            *
//    *************************************************************************/
//    rv = getFunctionList(&functionList);
//    CHECK_AND_LOG(" Get function list", rv == CKR_OK, rvToStr(rv), unload_pkcs11);

//    /*************************************************************************
//    * Инициализировать библиотеку                                            *
//    *************************************************************************/
//    rv = functionList->C_Initialize(&initArgs);
//    CHECK_AND_LOG(" C_Initialize", rv == CKR_OK, rvToStr(rv), unload_pkcs11);

//    /*************************************************************************
//    * Получить количество слотов c подключенными токенами                    *
//    *************************************************************************/
//    rv = functionList->C_GetSlotList(CK_TRUE, NULL_PTR, &slotCount);
//    CHECK_AND_LOG(" C_GetSlotList (number of slots)", rv == CKR_OK, rvToStr(rv), finalize_pkcs11);

//    CHECK_AND_LOG(" Checking available tokens", slotCount > 0, " No tokens available", finalize_pkcs11);

//    /*************************************************************************
//    * Получить список слотов c подключенными токенами                        *
//    *************************************************************************/
//    slotsSC = (CK_SLOT_ID_PTR)malloc(slotCount * sizeof(CK_SLOT_ID));
//    CHECK(" Memory allocation for slots", slotsSC != NULL, finalize_pkcs11);

//    rv = functionList->C_GetSlotList(CK_TRUE, slotsSC, &slotCount);
//    CHECK_AND_LOG(" C_GetSlotList", rv == CKR_OK, rvToStr(rv), free_slots);
//    printf(" Slots available: %d\n", (int)slotCount);

//    /*************************************************************************
//    * Открыть RW сессию в первом доступном слоте                             *
//    *************************************************************************/
//    rv = functionList->C_OpenSession(slotsSC[0], CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL_PTR, NULL_PTR, &session);
//    CHECK_AND_LOG(" C_OpenSession", rv == CKR_OK, rvToStr(rv), free_slots);

//    /*************************************************************************
//    * Выполнить аутентификацию Пользователя                                  *
//    *************************************************************************/
//    rv = functionList->C_Login(session, CKU_USER, USER_PIN, USER_PIN_LEN);
//    CHECK_AND_LOG(" C_Login", rv == CKR_OK, rvToStr(rv), close_session);
//    printf("Initialization has been completed successfully.\n");

//    /*************************************************************************
//    * Найти объект на токене (может быть несколько)                          *
//    *************************************************************************/
//    printf("\nFinding data object...\n");

//    r = findObjects(functionList, session, dataObjectTemplate, arraysize(dataObjectTemplate), &objects, &objectCount);
//    CHECK(" findObjects", r == 0, logout);

//    /*************************************************************************
//    * Удалить все найденные объекты (производится в цикле)                   *
//    *************************************************************************/
//    if (objectCount > 0) {
//        printf("\nDestroying objects...\n");

//        for (i = 0; i < objectCount; ++i) {
//            rv = functionList->C_DestroyObject(session, objects[i]);
//            CHECK_AND_LOG(" C_DestroyObject", rv == CKR_OK, rvToStr(rv), free_objects);
//        }

//        printf("Objects have been destroyed successfully.\n");
//    }

//    /*************************************************************************
//    * Выставить признак успешного завершения программы                       *
//    *************************************************************************/
//    errorCode = 0;

//    /*************************************************************************
//    * Выполнить действия для завершения работы с библиотекой PKCS#11         *
//    *************************************************************************/
//    printf("\nFinalizing... \n");

//    /*************************************************************************
//    * Очистить память, выделенную под объекты                                *
//    *************************************************************************/
//free_objects:
//    free(objects);

//    /*************************************************************************
//    * Сбросить права доступа                                                 *
//    *************************************************************************/
//logout:
//    rv = functionList->C_Logout(session);
//    CHECK_RELEASE_AND_LOG(" C_Logout", rv == CKR_OK, rvToStr(rv), errorCode);

//    /*************************************************************************
//    * Закрыть открытую сессию в слоте                                        *
//    *************************************************************************/
//close_session:
//    rv = functionList->C_CloseSession(session);
//    CHECK_RELEASE_AND_LOG(" C_CloseSession", rv == CKR_OK, rvToStr(rv), errorCode);

//    /*************************************************************************
//    * Очистить память, выделенную под слоты                                  *
//    *************************************************************************/
//free_slots:
//    free(slotsSC);

//    /*************************************************************************
//    * Деинициализировать библиотеку                                          *
//    *************************************************************************/
//finalize_pkcs11:
//    rv = functionList->C_Finalize(NULL_PTR);
//    CHECK_RELEASE_AND_LOG(" C_Finalize", rv == CKR_OK, rvToStr(rv), errorCode);

//    /*************************************************************************
//    * Выгрузить библиотеку из памяти                                         *
//    *************************************************************************/
//unload_pkcs11:
//    CHECK_RELEASE(" FreeLibrary", FreeLibrary(module), errorCode);

//exit:
//    if (errorCode) {
//        printf("\n\nSome error occurred. Sample failed.\n");
//    } else {
//        printf("\n\nSample has been completed successfully.\n");
//    }

//    return errorCode;
//}



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
    qmlRegisterType<certificateModel>("Custom.CertificateModel", 1, 0, "CertificateModel");
    QScopedPointer<HierarchyModel> hierarchyModel(new HierarchyModel);
    QQmlApplicationEngine qmlApplicationEngine;
//    deleteshit();
    qmlRegisterSingletonInstance("Custom.HierarchyModel", 1, 0, "HierarchyModel", hierarchyModel.get());
//    qmlEngine.rootContext()->setContextProperty("hierarchyModel", hierarchyModel);
    qmlApplicationEngine.load(QUrl(QStringLiteral("qrc:/hierarchylist.qml")));

    return app.exec();
}

