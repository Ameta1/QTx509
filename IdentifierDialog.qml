import QtQuick 2.0
import QtQuick.Controls 2.2

Dialog {
    id: newIdentifier

    property string prefix
    property string suffix
    property alias resultText: result.text

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
            text: qsTr("End file name: ") + newIdentifier.prefix + identifierTextField.text + newIdentifier.suffix
        }
    }
}
