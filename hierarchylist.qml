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
        id: headerRow
        Button {
            id: loadButton

            Layout.leftMargin: 10
            Layout.fillWidth: true
            implicitWidth: 0

            text: qsTr("Load hierarchy from file")
            onClicked: {
                fileDialog.open()
            }
        }

        Button {
            Layout.rightMargin: 10
            Layout.fillWidth: true
            implicitWidth: 0

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
            width: parent.width
        }
        ColumnLayout {
            id: buttonColumn
            spacing: 0
            width: parent.width
            Button {
                id: rootAndInter

                visible: HierarchyModel.threelevels
                enabled: !HierarchyModel.rootCreated
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
                enabled: HierarchyModel.rootCreated
                Layout.fillWidth: true
                text: qsTr("Generate new intermediate certificate")
                onClicked: {
                    intermediateDialog.open()
                }
            }
            Button {
                id: newLeaf

                visible: HierarchyModel.threelevels
                enabled: HierarchyModel.rootCreated
                Layout.fillWidth: true
                text: qsTr("Generate new leaf certificate")
                onClicked: {
                    leafDialog.open()
                }
            }
            Button {
                 id: root

                 visible: !HierarchyModel.threelevels
                 enabled: !HierarchyModel.rootCreated
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
                 enabled: HierarchyModel.rootCreated
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
            Button {
                id: deleteAll

                Layout.fillWidth: true
                text: qsTr("Delete all certificates on smart card")
                onClicked: {
                    deleteDialog.open()
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

    Dialog {
        id: deleteDialog

        anchors.centerIn: parent

        width: parent.width/2

        standardButtons: Dialog.Save | Dialog.Cancel

        contentItem: Text {
            id: sure
            text: qsTr("Are you sure?")
        }
        onAccepted: {
            exportCertModel.deleteAllCertsOnSC()
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
    Connections {
        id: reloadModels

        target: HierarchyModel
        function onCertListChanged() {
            ancestorCertModel.reload(HierarchyModel.folder, "*.intermediate.*")
            exportCertModel.reload(HierarchyModel.folder, "*.pem")
            certCombobox.currentIndex = 0
        }
    }
}
