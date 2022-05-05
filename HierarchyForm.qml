/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Certificate: https://www.qt.io/licensing/
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
** information use the certificate form at https://www.qt.io/certificate-us.
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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform 1.1
import "./"
ScrollView {
    property alias folder: folder
    property alias szCountry: szCountry
    property alias szProvince: szProvince
    property alias szCity: szCity
    property alias szOrganization: szOrganization
    property alias szCommon: szCommon
    property alias rootPass: rootPass
    property alias cypherSuite: cypherSuite
    property alias daysValid: daysValid
    property alias chainSuffix: caChainSuffix
    property alias rootSuffix: caRootSuffix

    ColumnLayout {
        id: editColumn
        spacing: 10

        CustomTextField {
            id: folder
            text: folderDialog.folder + "/" + szOrganization.text
            infoText: qsTr("Folder")
            onPressed: folderDialog.open()
        }
        CustomTextField {
            id: szCountry
            text: "RU"
            infoText: qsTr("Country code:")
        }
        CustomTextField {
            id: szProvince
            text: "77"
            infoText: qsTr("Region code:")
        }
        CustomTextField {
            id: szCity
            text: "Zelenograd"
            infoText: qsTr("City:")
        }
        CustomTextField {
            id: szOrganization
            text: "MIET"
            infoText: qsTr("Organization:")
        }
        CustomTextField {
            id: szCommon
            text: "localhost"
            infoText: qsTr("Common:")
        }
        CustomTextField {
            id: rootPass
            text: "1234"
            infoText: qsTr("Root CA Password:")
        }
        Label {
            text: qsTr("Cypher suite:")
        }
        ComboBox {
            id: cypherSuite
            implicitWidth: 700
        }
        CustomTextField {
            id: daysValid
            text: "3650"
            infoText: qsTr("Days valid:")
            validator: IntValidator {}
        }
        CustomTextField {
            id: caChainSuffix
            text: szOrganization.text + ".chain.ca"
            infoText: qsTr("CA chain suffix:")
        }
        CustomTextField {
            id: caRootSuffix
            text: szOrganization.text + ".root.ca"
            infoText: qsTr("CA root suffix:")
        }

        FolderDialog {
            id: folderDialog
            title: "Please choose a folder"
            folder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        }
    }
}
