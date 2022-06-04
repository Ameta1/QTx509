import QtQuick 2.0
import QtQuick.Controls 2.0
import Custom.HierarchyModel 1.0
import Custom.SettingsContainer 1.0

ApplicationWindow {
    width: 800
    height: 1000
    visible: true

    SettingTextField {
        id: coolTextField
    }

    SettingsContainer {
        id: settingsContainer
        country: coolTextField.text
    }

    Button {

        onClicked: HierarchyModel.newData(settingsContainer)
    }
}
