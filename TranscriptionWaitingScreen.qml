import QtQuick
import QtQuick.Controls
import Transcription

Rectangle {
    id: test

    anchors.fill: parent

    color: "#2f2f2f"

    property var notes: undefined

    MouseArea {
        anchors.fill: parent

        onClicked: {

            //            stackview.push(pianoroll)
        }
    }

    Component.onCompleted: {
        anchors.fill = undefined
    }

    Column {
        anchors.centerIn: parent

        Text {
            text: "Waiting for transcription"
            font.pixelSize: 20
            color: "#ffffff"

            anchors.horizontalCenter: parent.horizontalCenter
        }

        BusyIndicator {

            width: 50
            height: 50

            running: true

            //            color: "#ffffff"
            palette.dark: "#ffffff"

            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
