import QtQuick

Item {

    property int octaveNumber: undefined

    id: octaveContainer

    height: octaveColumn.height
    width: octaveColumn.width

    ListModel {
        id: octaveModel

        ListElement {
            noteName: "B"
            noteColor: "white"
        }
        ListElement {
            noteName: "A#/Bb"
            noteColor: "black"
        }
        ListElement {
            noteName: "A"
            noteColor: "white"
        }
        ListElement {
            noteName: "G#/Ab"
            noteColor: "black"
        }
        ListElement {
            noteName: "G"
            noteColor: "white"
        }
        ListElement {
            noteName: "F#/Gb"
            noteColor: "black"
        }
        ListElement {
            noteName: "F"
            noteColor: "white"
        }
        ListElement {
            noteName: "E"
            noteColor: "white"
        }
        ListElement {
            noteName: "D#/Eb"
            noteColor: "black"
        }
        ListElement {
            noteName: "D"
            noteColor: "white"
        }
        ListElement {
            noteName: "C#/Db"
            noteColor: "black"
        }
        ListElement {
            noteName: "C"
            noteColor: "white"
        }
    }

    Column {
        id: octaveColumn

        spacing: 4

        Repeater {
            id: aaa
            model: octaveModel

            delegate: KeyboardNote {
                noteColor: model.noteColor
                noteName: model.noteName
                noteOctave: octaveContainer.octaveNumber
            }
        }
    }
}
