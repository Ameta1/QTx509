#include "certificatemodel.h"
#include <QDir>
certificateModel::certificateModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int certificateModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return certificateList.count();
}

QVariant certificateModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < rowCount())
        switch (role) {
        case StringRole: return certificateList.at(index.row()).value("CertString");
        case ForExportRole: return certificateList.at(index.row()).value("ForExport");
        default: return QVariant();
    }
    return QVariant();
}

QHash<int, QByteArray> certificateModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { StringRole, "CertString" },
        { ForExportRole, "ForExport"}
    };
    return roles;
}

void certificateModel::reload(QString folder, QString filter = "*.*")
{
    QDir certDir(folder + "/certs/");
    QStringList certStringList = certDir.entryList(QStringList(filter),QDir::Files);
    beginResetModel();
    certificateList.clear();
    for (const QString& certString: certStringList) {
        QVariantMap certInfo;
        certInfo.insert("CertString", certString);
        certInfo.insert("ForExport", false);
        qDebug() <<certDir.path() + certString;
        certInfo.insert("FullPath", certDir.path() + certString);
        certificateList.append(certInfo);
    }
    endResetModel();
}

void certificateModel::checkForExport(const int index)
{
    certificateList[index]["ForExport"] = true;
}

void certificateModel::uncheckForExport(const int index)
{
    certificateList[index]["ForExport"] = false;
}

int certificateModel::exportToSmartCard()
{
    HMODULE module;                                   // Хэндл загруженной библиотеки PKCS#11
    CK_SESSION_HANDLE session;                        // Хэндл открытой сессии

    CK_FUNCTION_LIST_PTR functionList;                // Указатель на список функций PKCS#11, хранящийся в структуре CK_FUNCTION_LIST
    CK_C_GetFunctionList getFunctionList;             // Указатель на функцию C_GetFunctionList

    CK_SLOT_ID_PTR slotsSC;                             // Массив идентификаторов слотов
    CK_ULONG slotCount;                               // Количество идентификаторов слотов в массиве

    CK_OBJECT_HANDLE dataObject;                      // Хэндл объекта с бинарными данными

    CK_RV rv;                                         // Код возврата. Могут быть возвращены только ошибки, определенные в PKCS#11

    int errorCode = 1;                                // Флаг ошибки

    /*************************************************************************
    * Загрузить библиотеку                                                   *
    *************************************************************************/
    module = LoadLibrary(PKCS11ECP_LIBRARY_NAME);
    CHECK(" LoadLibrary", module != NULL, exit);

    /*************************************************************************
    * Получить адрес функции запроса структуры с указателями на функции      *
    *************************************************************************/
    getFunctionList = (CK_C_GetFunctionList)GetProcAddress(module, "C_GetFunctionList");
    CHECK(" GetProcAddress", getFunctionList != NULL, unload_pkcs11);

    /*************************************************************************
    * Получить структуру с указателями на функции                            *
    *************************************************************************/
    rv = getFunctionList(&functionList);
    CHECK_AND_LOG(" Get function list", rv == CKR_OK, rvToStr(rv), unload_pkcs11);

    /*************************************************************************
    * Инициализировать библиотеку                                            *
    *************************************************************************/
    rv = functionList->C_Initialize(&initArgs);
    CHECK_AND_LOG(" C_Initialize", rv == CKR_OK, rvToStr(rv), unload_pkcs11);

    /*************************************************************************
    * Получить количество слотов c подключенными токенами                    *
    *************************************************************************/
    rv = functionList->C_GetSlotList(CK_TRUE, NULL_PTR, &slotCount);
    CHECK_AND_LOG(" C_GetSlotList (number of slots)", rv == CKR_OK, rvToStr(rv), finalize_pkcs11);

    CHECK_AND_LOG(" Checking available tokens", slotCount > 0, " No tokens available", finalize_pkcs11);

    /*************************************************************************
    * Получить список слотов c подключенными токенами                        *
    *************************************************************************/
    slotsSC = (CK_SLOT_ID_PTR)malloc(slotCount * sizeof(CK_SLOT_ID));
    CHECK(" Memory allocation for slots", slotsSC != NULL, finalize_pkcs11);

    rv = functionList->C_GetSlotList(CK_TRUE, slotsSC, &slotCount);
    CHECK_AND_LOG(" C_GetSlotList", rv == CKR_OK, rvToStr(rv), free_slots);
    printf(" Slots available: %d\n", (int)slotCount);

    /*************************************************************************
    * Открыть RW сессию в первом доступном слоте                             *
    *************************************************************************/
    rv = functionList->C_OpenSession(slotsSC[0], CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL_PTR, NULL_PTR, &session);
    CHECK_AND_LOG(" C_OpenSession", rv == CKR_OK, rvToStr(rv), free_slots);

    /*************************************************************************
    * Выполнить аутентификацию Пользователя                                  *
    *************************************************************************/
    rv = functionList->C_Login(session, CKU_USER, USER_PIN, USER_PIN_LEN);
    CHECK_AND_LOG(" C_Login", rv == CKR_OK, rvToStr(rv), close_session);
    printf("Initialization has been completed successfully.\n");

    for (const auto& certificate : certificateList) {
        if (certificate.value("ForExport").toBool()) {
            QFile certFile(certificate.value("FullPath").toString());
            certFile.open(QIODevice::ReadOnly);
            QByteArray data = certFile.readAll();
            CK_ATTRIBUTE dataObjectTemplate[] =
            {
                { CKA_CLASS, &dataObject, sizeof(dataObject) },                       // Класс - бинарные данные
                { CKA_LABEL, dataObjectLabel, sizeof(dataObjectLabel) - 1 },          // Метка объекта с данными
                { CKA_APPLICATION, applicationLabel, sizeof(applicationLabel) - 1 },  // Описание приложения, которое записывает данные
                { CKA_TOKEN, &attributeTrue, sizeof(attributeTrue) },                 // Данные являются объектом токена
                { CKA_PRIVATE, &attributeTrue, sizeof(attributeTrue) },               // Данные доступны только после аутентификации на токене
                { CKA_VALUE, data.data(), sizeof(data.data()) }                                     // Данные для записи
            };

            rv = functionList->C_CreateObject(session, dataObjectTemplate, arraysize(dataObjectTemplate), &dataObject);
            CHECK_AND_LOG(" C_CreateObject", rv == CKR_OK, rvToStr(rv), logout);
        }
    }

    /*************************************************************************
    * Выставить признак успешного завершения программы                       *
    *************************************************************************/
    errorCode = 0;

    /*************************************************************************
    * Выполнить действия для завершения работы с библиотекой PKCS#11         *
    *************************************************************************/
    printf("\nFinalizing... \n");

    /*************************************************************************
    * Сбросить права доступа                                                 *
    *************************************************************************/
logout:
    rv = functionList->C_Logout(session);
    CHECK_RELEASE_AND_LOG(" C_Logout", rv == CKR_OK, rvToStr(rv), errorCode);

    /*************************************************************************
    * Закрыть открытую сессию в слоте                                        *
    *************************************************************************/
close_session:
    rv = functionList->C_CloseSession(session);
    CHECK_RELEASE_AND_LOG(" C_CloseSession", rv == CKR_OK, rvToStr(rv), errorCode);

    /*************************************************************************
    * Очистить память, выделенную под слоты                                  *
    *************************************************************************/
free_slots:
    free(slotsSC);

    /*************************************************************************
    * Деинициализировать библиотеку                                          *
    *************************************************************************/
finalize_pkcs11:
    rv = functionList->C_Finalize(NULL_PTR);
    CHECK_RELEASE_AND_LOG(" C_Finalize", rv == CKR_OK, rvToStr(rv), errorCode);

    /*************************************************************************
    * Выгрузить библиотеку из памяти                                         *
    *************************************************************************/
unload_pkcs11:
    CHECK_RELEASE(" FreeLibrary", FreeLibrary(module), errorCode);

exit:
    if (errorCode) {
        printf("\n\nSome error occurred. Sample failed.\n");
    } else {
        printf("\n\nSample has been completed successfully.\n");
    }

    return errorCode;
}

