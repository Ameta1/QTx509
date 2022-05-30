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
import QtQuick.Layouts 1.2
import Custom.HierarchyModel 1.0
import Custom.CertificateModel 1.0
ApplicationWindow {
    id: window

    width: 800
    height: 1000
    visible: true
    title: qsTr("Hierarchy List")

    header: RowLayout {
        Button {
            Layout.fillWidth: true
            id: loadButton
            text: qsTr("Load hierarchy from file")
            onClicked: {
                fileDialog.open()
            }
        }

        Button {
            Layout.fillWidth: true
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
            HierarchyModel.load(file)
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
        }

        onAccepted: {
            form.accepted()
            HierarchyModel.createNew()
            loadedView.visible = true
        }
    }

    Column {
        id: loadedView
        visible: false
        padding: 10
        width: window.width - padding*2
        HierarchyView {
            id: hierarchyView
        }
        onVisibleChanged: {
            rootAndInter.enabled = !HierarchyModel.rootCreated()
            root.enabled = !HierarchyModel.rootCreated()
            newIntermediate.enabled = HierarchyModel.rootCreated()
            newLeaf.enabled = HierarchyModel.rootCreated()
            end.enabled = HierarchyModel.rootCreated()
            ancestorCertModel.reload(HierarchyModel.folder, "*.intermediate.*")
            exportCertModel.reload(HierarchyModel.folder, "*.pem")
            certCombobox.currentIndex = 0

        }
        ColumnLayout {
            id: threeLevelColumn
            spacing: 0
            width: parent.width
            Button {
                id: rootAndInter

                visible: HierarchyModel.threelevels
                Layout.fillWidth: true
                width: window.width
                text: qsTr("Generate root and intermediate certificates")
                onClicked: {
                    HierarchyModel.createRootAndIntermediate()
                    successPopup.open()
                    rootAndInter.enabled = false
                    newIntermediate.enabled = true
                    newLeaf.enabled = true
                }
            }
            Button {
                id: newIntermediate

                visible: HierarchyModel.threelevels
                Layout.fillWidth: true
                text: qsTr("Generate new intermediate certificate")
                onClicked: {
                    intermediateDialog.open()
                }
            }
            Button {
                id: newLeaf

                visible: HierarchyModel.threelevels
                Layout.fillWidth: true
                text: qsTr("Generate new leaf certificate")
                onClicked: {
                    leafDialog.open()
                }
            }
            Button {
                 id: root

                 visible: !HierarchyModel.threelevels
                 Layout.fillWidth: true
                 text: qsTr("Generate root certificate")
                 onClicked: {
                     HierarchyModel.createRoot()
                     end.enabled = true
                     root.enabled = false
                }
            }
            Button {
                 id: end

                 visible: !HierarchyModel.threelevels
                 Layout.fillWidth: true
                 text: qsTr("Generate end certificate")
                 onClicked: {
                     intermediateDialog.open()
                }
            }
            Button {
                 id: exportToCard

                 Layout.fillWidth: true
                 text: qsTr("Export to smart card")
                 onClicked: {
                     exportDialog.open()
                 }
            }
        }
    }

    Dialog {
        id: intermediateDialog

        anchors.centerIn: parent

        standardButtons: Dialog.Save | Dialog.Cancel

        contentItem: Column {
            Text {
                id: prompt
                text: qsTr("Please enter new identifier for this certificate")
            }
            SettingTextField {
                id: identifierTextField
                infoText: qsTr("New identifier")
            }
            Text {
                id: result
                text: qsTr("End file name: ") + identifierTextField.text + ".cert.pem"
            }
        }
        onAccepted: {
            HierarchyModel.createAdditionalIntermediate(identifierTextField.text)
            successPopup.open()
        }
    }
    Platform.FileDialog {
        id: certDialog
        options: Platform.FileDialog.ReadOnly
        fileMode: Platform.FileDialog.OpenFile
        folder: HierarchyModel.folder + "/" + "certs"
        nameFilters: ["Text files (*.crt)"]
        onAccepted: {
            HierarchyModel.load(file)
            loadedView.visible = true
        }
    }

    Dialog {
        id: leafDialog

        anchors.centerIn: parent

        standardButtons: Dialog.Save | Dialog.Cancel

        contentItem: ColumnLayout {
            Label {
                id: certComboboxLabel

                text: qsTr("Parent certificate")
            }

            ComboBox {
                id: certCombobox
                textRole: "CertString"
                Layout.fillWidth: true
                model: CertificateModel {
                    id: ancestorCertModel
                }
            }

            Text {
                id: leafPrompt

                Layout.fillWidth: true
                text: qsTr("Please enter new identifier for this certificate")
            }
            SettingTextField {
                id: leafIdentifierTextField

                Layout.fillWidth: true
                infoText: qsTr("New identifier")
            }
            Text {
                id: leafResult

                Layout.fillWidth: true
                text: qsTr("End file name: ") + leafIdentifierTextField.text + ".cert.pem"
            }
        }

        onAccepted: {
            HierarchyModel.createLeaf(leafIdentifierTextField.text, certCombobox.currentValue)
            successPopup.open()
        }
    }

    Dialog {
        id: exportDialog

        anchors.centerIn: parent

        width: parent.width/2

        standardButtons: Dialog.Save | Dialog.Cancel

        contentItem: ColumnLayout {
            Label {
                id: exportLabel

                text: qsTr("Choose certificates to export:")
            }

            ListView {
                id: exportListView

                height: 200
                boundsBehavior: Flickable.StopAtBounds
                Layout.fillWidth: true
                model: CertificateModel {
                    id: exportCertModel
                }
                delegate: CheckDelegate {
                    width: exportListView.width
                    text: CertString
                    checked: ForExport
                    onClicked: {
                        if (checkState === Qt.Checked) {
                            exportCertModel.checkForExport(index)
                        }
                        else {
                             exportCertModel.uncheckForExport(index)
                        }
                    }
                }
            }
        }
        onAccepted: {
            exportCertModel.exportToSmartCard()
        }
    }

    Popup {
        id: successPopup
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)
        width: window.width/2
        height: window.height/2
        modal: true
        focus: true

        contentItem: Text {
            id: popupText
            font.pointSize: 30
            text: qsTr("Success!")
        }
    }
}
