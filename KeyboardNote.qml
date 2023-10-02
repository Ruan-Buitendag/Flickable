import QtQuick

Item {

    id: note

    property string noteName: "note"
    property color noteColor: "white"
    property int noteOctave: undefined

    height: 20
    width: height * 4.5

    Rectangle {
        height: parent.height
        width: parent.width / 4
        color: noteColor
    }
    Rectangle {
        id: noteRectangle

        anchors.fill: parent
        color: noteColor

        radius: height / 4

        Text {
            id: noteText

            text: noteName + noteOctave
            color: Qt.colorEqual(noteColor, "black") ? "white" : "black"

            font.pixelSize: parent.height - 3

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            anchors.rightMargin: parent.height / 4
        }
    }
}
