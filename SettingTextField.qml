import QtQuick 2.0
import QtQuick.Controls 2.2

TextField {
    id: settingTextField
    property string infoText

    selectByMouse: true

    placeholderText: qsTr("Enter value")

    background: Rectangle {
        radius: 3
    }

    width: parent.width
    topPadding: info.height + 5
    Text {
        id: info
        text: infoText
    }
}
