import QtQuick
import QtQuick.Controls

Item {
    anchors.bottom: parent.bottom
    anchors.right: parent.right

    width: 50
    height: 50

    SpinBox {
        value: 10

        editable: true

        from: 1
        to: 20

        onValueChanged: {
            console.log("value changed to " + value)
            transcription.transcriptionIterations = value
        }
    }
}
