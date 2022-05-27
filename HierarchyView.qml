import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Custom.HierarchyModel 1.0
import "./"

ScrollView {
    ColumnLayout {
        id: editColumn
        spacing: 10

        SettingTextField {
            id: country
            text: HierarchyModel.country
            infoText: qsTr("Country code:")
            readOnly: true
        }
        SettingTextField {
            id: province
            text: HierarchyModel.province
            infoText: qsTr("Region code:")
        }
        SettingTextField {
            id: city
            text: HierarchyModel.city
            infoText: qsTr("City:")
            readOnly: true
        }
        SettingTextField {
            id: organization
            text: HierarchyModel.organization
            infoText: qsTr("Organization:")
            readOnly: true
        }
        SettingTextField {
            id: common
            text: HierarchyModel.common
            infoText: qsTr("Common:")
            readOnly: true
        }
        SettingTextField {
            id: rootCApassword
            text: HierarchyModel.rootCApassword
            infoText: qsTr("Root CA Password:")
            readOnly: true
        }
        SettingTextField {
            id: cypherSuite
            text: HierarchyModel.cypherSuite
            infoText: qsTr("Cypher suite:")
            readOnly: true
        }
        SettingTextField {
            id: daysValid
            text: HierarchyModel.daysValid
            infoText: qsTr("Days valid:")
            readOnly: true
        }
        SettingTextField {
            id: chainCASuffix
            text: HierarchyModel.chainCASuffix
            infoText: qsTr("CA chain suffix:")
            readOnly: true
        }
        SettingTextField {
            id: rootCAsuffix
            text: HierarchyModel.rootCAsuffix
            infoText: qsTr("CA root suffix:")
            readOnly: true
        }
        SettingTextField {
            id: intermediateCAPassword
            visible: HierarchyModel.threelevels
            text: HierarchyModel.intermediateCAPassword
            infoText: qsTr("Intermediate CA Password:")
            readOnly: true
        }
        SettingTextField {
            id: intermediateCASuffix
            visible: HierarchyModel.threelevels
            text: HierarchyModel.intermediateCASuffix
            infoText: qsTr("intermediate CA suffix:")
            readOnly: true
        }
    }
}
