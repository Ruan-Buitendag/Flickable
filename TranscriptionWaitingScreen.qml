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

        //        transcription.startTranscription()
        //        stackview.push(pianoroll)
    }

    DebugBox {

        anchors.centerIn: parent

        Column {

            DebugBox {
                Text {
                    text: "Waiting for transcription"
                    font.pixelSize: 20
                    color: "#ffffff"

                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
            DebugBox {
                BusyIndicator {

                    width: 50
                    height: 50

                    running: true

                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    //    BackButton {}
}
