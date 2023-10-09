import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Transcription

import QtCore

Rectangle {
    anchors.fill: parent

    Text {
        anchors.centerIn: parent
        text: "Song loading"
    }

    FileDialog {
        id: wavFileDialog
        currentFolder: "file:///C:/Users/ruanb/OneDrive/Desktop/Piano Transcripton/Piano transcription/MAPS"

        onAccepted: {
            transcription.songName = selectedFile
            console.log("Wav file: " + selectedFile)
        }
    }

    FileDialog {
        id: transcriptionFileDialog

        currentFolder: "file:///C:/Users/ruanb/OneDrive/Desktop/Piano Transcripton/Piano transcription/MAPS"

        onAccepted: {
            transcription.transcriptionFile = selectedFile
            console.log("Transcription file: " + selectedFile)
        }
    }

    Rectangle {

        width: 100
        height: 100

        color: "red"

        //        anchors.centerIn: parent
        MouseArea {
            anchors.fill: parent

            onClicked: transcriptionFileDialog.open()
        }
    }

    Rectangle {
        x: 400

        width: 100
        height: 100

        color: "orange"

        //        anchors.centerIn: parent
        MouseArea {
            anchors.fill: parent

            onClicked: wavFileDialog.open()
        }
    }

    Rectangle {
        x: 500

        width: 100
        height: 100

        color: "green"

        //        anchors.centerIn: parent
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