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

import QtQuick
import QtQuick.Controls
import Qt.labs.platform 1.1 as Platform

ApplicationWindow {
    id: window

    width: 800
    height: 1000
    visible: true
    title: qsTr("Hierarchy List")

    header: Row{
        Button {
            id: loadButton
            text: qsTr("Load hierarchy from file")
            onClicked: {
                fileDialog.open()
            }
        }

        Button{
            id: saveButton
            text: qsTr("Make a new hierarchy")
            onClicked: {
                hierarchyDialog.open()
            }
        }
    }

    Platform.FileDialog {
        id: fileDialog
        fileMode: Platform.FileDialog.OpenFile
        folder: Platform.StandardPaths.standardLocations(Platform.StandardPaths.HomeLocation)[0]
        nameFilters: ["Text files (*.json)"]
        onAccepted: {
            hierarchyView.model.load(file)
            loadedView.visible = true
        }
    }


    Dialog {
        id: hierarchyDialog

        width: parent.width
        height: parent.height


        focus: true
        modal: true

        title: qsTr("New Hierarchy")
        standardButtons: Dialog.Save | Dialog.Cancel

        contentItem: HierarchyForm {
            id: form
            cypherSuite.model: hierarchyView.model.cypherSuites()
        }

        onAccepted: {
            hierarchyView.model.set(form.szCountry.text,
                                    form.szProvince.text,
                                    form.szCity.text,
                                    form.szOrganization.text,
                                    form.szCommon.text,
                                    form.rootPass.text,
                                    form.cypherSuite.currentText,
                                    form.daysValid.text,
                                    form.chainSuffix.text,
                                    form.rootSuffix.text,
                                    form.folder.text)
            hierarchyView.model.createNew()
            loadedView.visible = true
        }
    }

    Item {
        id: loadedView
        visible: false
        HierarchyView {
            id: hierarchyView
            anchors.fill: parent
            footer: Row {
                Button {
                    id: rootAndInter

                    text: qsTr("Generate root and intermediate certificates")
                    onClicked: {
                        hierarchyView.model.createRootAndIntermediate()
                    }
                }
            }
        }
    }

    Popup {
        id: successPopup
        anchors.centerIn: window.center
        width: window.width/2
        height: window.height/2
        modal: true
        focus: true

        contentItem: Text {
            id: popupText
            text: qsTr("Success!")
        }
    }

}
