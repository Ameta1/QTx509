import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Custom.HierarchyModel 1.0
import "./"

GridLayout {
    id: viewGrid
    columns: 2
    columnSpacing: 10
    width: parent.width
    SettingTextField {
        id: country

        Layout.fillWidth: true
        implicitWidth: 0
        text: HierarchyModel.country
        infoText: qsTr("Country code:")
        readOnly: true
    }
    SettingTextField {
        id: province

        Layout.fillWidth: true
        implicitWidth: 0
        text: HierarchyModel.province
        infoText: qsTr("Region code:")
    }
    SettingTextField {
        id: city

        Layout.fillWidth: true
        implicitWidth: 0
        text: HierarchyModel.city
        infoText: qsTr("City:")
        readOnly: true
    }
    SettingTextField {
        id: organization

        Layout.fillWidth: true
        implicitWidth: 0
        text: HierarchyModel.organization
        infoText: qsTr("Organization:")
        readOnly: true
    }
    SettingTextField {
        id: common

        Layout.fillWidth: true
        implicitWidth: 0
        text: HierarchyModel.common
        infoText: qsTr("Common:")
        readOnly: true
    }
    SettingTextField {
        id: rootCApassword

        Layout.fillWidth: true
        implicitWidth: 0
        text: HierarchyModel.rootCApassword
        infoText: qsTr("Root CA Password:")
        readOnly: true
    }
    SettingTextField {
        id: cypherSuite

        Layout.fillWidth: true
        implicitWidth: 0
        text: HierarchyModel.cypherSuite
        infoText: qsTr("Cypher suite:")
        readOnly: true
    }
    SettingTextField {
        id: daysValid

        Layout.fillWidth: true
        implicitWidth: 0
        text: HierarchyModel.daysValid
        infoText: qsTr("Days valid:")
        readOnly: true
    }
    SettingTextField {
        id: chainCASuffix

        Layout.fillWidth: true
        implicitWidth: 0
        text: HierarchyModel.chainCASuffix
        infoText: qsTr("CA chain suffix:")
        readOnly: true
    }
    SettingTextField {
        id: rootCAsuffix

        Layout.fillWidth: true
        implicitWidth: 0
        text: HierarchyModel.rootCAsuffix
        infoText: qsTr("CA root suffix:")
        readOnly: true
    }
    SettingTextField {
        id: intermediateCAPassword

        Layout.fillWidth: true
        implicitWidth: 0
        visible: HierarchyModel.threelevels
        text: HierarchyModel.intermediateCAPassword
        infoText: qsTr("Intermediate CA Password:")
        readOnly: true
    }
    SettingTextField {
        id: intermediateCASuffix

        Layout.fillWidth: true
        implicitWidth: 0
        visible: HierarchyModel.threelevels
        text: HierarchyModel.intermediateCASuffix
        infoText: qsTr("intermediate CA suffix:")
        readOnly: true
    }
}

