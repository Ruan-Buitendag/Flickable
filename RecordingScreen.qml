import QtQuick
import QtQuick.Controls
import QtMultimedia

Rectangle {
    anchors.fill: parent

    color: "#2f2f2f"

    Column {
        anchors.centerIn: parent

        spacing: 10

        Text {
            //            anchors.horizontalCenter: parent.horizontalCenter
            text: "Press button to record"
            font.pixelSize: 20
            color: "#ffffff"
        }

        Item {
            width: 200
            height: 200

            //            DebugBox {
            Rectangle {
                id: record_button

                width: 75
                height: width

                radius: width / 2

                color: "red"

                x: parent.width / 2 - width / 2
                y: parent.height / 2 - height / 2

                property bool recording: false

                Rectangle {
                    id: stop_button

                    width: 25
                    height: width

                    radius: 5

                    color: "#2f2f2f"

                    anchors.centerIn: parent

                    opacity: record_button.recording ? 1 : 0

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 250
                            easing.type: Easing.InOutQuad
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        record_button.recording = !record_button.recording

                        if (!record_button.recording) {
                            transcribe_button.visible = true
                            transcribe_button.opacity = 1

                            transcription.stopRecording()
                        } else {
                            transcribe_button.visible = false
                            transcribe_button.opacity = 0

                            transcription.startRecording()
                        }
                    }
                }
            }

            Rectangle {
                id: transcribe_button

                opacity: 0

                visible: false

                width: 50
                height: width

                color: "green"

                x: 300
                y: record_button.y

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        doTranscription("live")
                        console.log("Transcribing")
                    }
                }

                Behavior on opacity {
                    NumberAnimation {
                        duration: 250
                        easing.type: Easing.InOutQuad
                    }
                }

                Behavior on visible {
                    NumberAnimation {
                        duration: 250
                        easing.type: Easing.InOutQuad
                    }
                }

                Behavior on x {
                    NumberAnimation {
                        duration: 250
                        easing.type: Easing.InOutQuad
                    }
                }

                //                }
            }
        }
    }

    BackButton {}

    Component.onCompleted: {
        anchors.fill = undefined
    }

    MediaDevices {
        id: mediaDevices
    }

    AudioDeviceSelector {
        anchors.bottom: parent.bottom
    }

    IterationSelector {}

    Rectangle {
        x: 300
        y: 300

        width: 100
        height: 100

        color: "blue"

        MouseArea {
            anchors.fill: parent

            onClicked: {
                transcription.playRecording()
            }
        }
    }
}
