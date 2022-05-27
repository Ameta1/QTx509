import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform 1.1
import Custom.HierarchyModel 1.0
import "./"

//Component.onCompleted: text=HierarchyModel.intermediateCASuffix
//BREAKS BINDINGS LIKE
//text: organization.text + ".intermediate"

ScrollView {
    id: scrollView
    signal accepted()
    ColumnLayout {
        id: editColumn
        spacing: 10
        SettingTextField {
            id: folder
            text: folderDialog.folder + "/" + organization.text + "/"
            infoText: qsTr("Folder")
            onPressed: folderDialog.open()
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.folder = folder.text}
            }
        }
        SettingTextField {
            id: country
            infoText: qsTr("Country code:")
            text: "RU"
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.country = country.text}
            }
        }
        SettingTextField {
            id: province
            infoText: qsTr("Region code:")
            text: "77"
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.province = province.text}
            }
        }
        SettingTextField {
            id: city
            infoText: qsTr("City:")
            text: "Zelenograd"
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.city = city.text}
            }
        }
        SettingTextField {
            id: organization
            infoText: qsTr("Organization:")
            text: "Miet"
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.organization = organization.text}
            }
        }
        SettingTextField {
            id: common
            infoText: qsTr("Common:")
            text: "TCS"
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.common = common.text}
            }
        }
        SettingTextField {
            id: rootCApassword
            infoText: qsTr("Root CA Password:")
            text: "1234"
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.rootCApassword = rootCApassword.text}
            }
        }
        Label {
            text: qsTr("Cypher suite:")
        }
        ComboBox {
            id: cypherSuite
            model: ["ecdsa, prime256v1, sha256",
                    "rsa, 4096, sha256",
                    "gostr34102012_256a, GC256A, STRIBOG_256",
                    "gostr34102012_256b, GC256B, STRIBOG_256",
                    "gostr34102012_256c, GC256C, STRIBOG_256"]
            implicitWidth: 700
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.cypherSuite = cypherSuite.currentValue}
            }
        }
        SettingTextField {
            id: daysValid
            infoText: qsTr("Days valid:")
            text: "3650"
            validator: IntValidator {}
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.daysValid = daysValid.text}
            }
        }
        SettingTextField {
            id: chainCASuffix
            text: organization.text + ".chain.ca"
            infoText: qsTr("CA chain suffix:")
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.chainCASuffix = chainCASuffix.text}
            }
        }
        SettingTextField {
            id: rootCAsuffix
            text: organization.text + ".root.ca"
            infoText: qsTr("CA root suffix:")
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.rootCAsuffix = rootCAsuffix.text}
            }
        }
        Label {
            text: qsTr("Hierarchy levels")
        }
        Row {
            Label {
                text: "2"
                font.pointSize: 15
                visible: !stageSelector.checked
            }
            Switch {
                id: stageSelector
                checked: false
                onClicked: {
                    HierarchyModel.threelevels = stageSelector.checked
                    intermediateCAPassword.visible = stageSelector.checked
                    intermediateCASuffix.visible = stageSelector.checked
                }
            }
            Label {
                text: "3"
                font.pointSize: 15
                visible: stageSelector.checked
            }
        }
        SettingTextField {
            id: intermediateCAPassword
            visible: false
            infoText: qsTr("Intermediate CA Password:")
            text: "1234"
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.intermediateCAPassword = intermediateCAPassword.text}
            }
        }
        SettingTextField {
            id: intermediateCASuffix
            visible: false
            text: organization.text + ".intermediate"
            infoText: qsTr("intermediate CA suffix:")
            Connections {
                target: scrollView
                function onAccepted(){ HierarchyModel.intermediateCASuffix = intermediateCASuffix.text}
            }
        }
        FolderDialog {
            id: folderDialog
            title: "Please choose a folder"
            folder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        }
    }
}
