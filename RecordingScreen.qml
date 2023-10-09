import QtQuick
import QtQuick.Controls

Rectangle {
    anchors.fill: parent

    color: "#2f2f2f"

    Text {
        anchors.centerIn: parent
        text: "Press button to record"
        font.pixelSize: 20
        color: "#ffffff"
    }

    Rectangle {
        width: 100
        height: 100

        anchors.centerIn: parent

        MouseArea {
            anchors.fill: parent
            onClicked: {
                doTranscription()
            }
        }
    }

    BackButton {}

    Component.onCompleted: {
        anchors.fill = undefined
    }
}
