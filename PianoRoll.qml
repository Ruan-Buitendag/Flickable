import QtQuick
import Transcription

Rectangle {
    id: background

    anchors.fill: parent

    color: "#2f2f2f"

    Flickable {

        x: keyboard.width

        id: note_flickable

        width: parent.width - keyboard.width
        height: parent.height

        property int totalwidth: parent.width + 200

        maximumFlickVelocity: 1000

        contentHeight: keyboard.height
        contentWidth: totalwidth
        Rectangle {
            anchors.fill: parent
            id: flickable_background
            color: "#2f2f2f"

            CustomGrid {
                id: grid

                blockWidth: 200
                blockHeight: 24

                columns: 20
                rows: 88
            }

            Repeater {
                model: transcription.notes

                NoteActivation {
                    function scaleOnset(onset) {

                        var scaledOnset = onset * grid.blockWidth

                        return scaledOnset
                    }

                    function scaleMidiValue(midiValue) {

                        var y = keyboard.height - ((midiValue - 21) * 24) + 4

                        return clipY(y)
                    }

                    function clipY(y) {
                        if (y < 0) {
                            return 0
                        } else if (y > keyboard.height) {
                            return keyboard.height
                        } else {
                            return y
                        }
                    }

                    width: grid.blockWidth / 2

                    x: scaleOnset(modelData.onset)
                    y: scaleMidiValue(modelData.midiValue)

                    onXChanged: {
                        if (x > note_flickable.contentWidth) {
                            note_flickable.totalwidth = x + 200
                        }
                    }
                }
            }
        }

        onContentYChanged: {
            keyboard_flickable.contentY = contentY

            grid.columns = Math.ceil(
                        note_flickable.contentWidth / grid.blockWidth)
        }

        onContentXChanged: {
            timestep_flickable.contentX = contentX
        }
    }

    Flickable {
        id: keyboard_flickable

        interactive: false

        width: keyboard.width
        height: note_flickable.height

        contentHeight: keyboard.height
        contentWidth: keyboard.width

        maximumFlickVelocity: 1000

        Keyboard {
            id: keyboard
        }
    }

    Flickable {
        id: timestep_flickable

        x: keyboard.width

        interactive: false

        width: note_flickable.width
        height: 100

        Repeater {
            model: grid.columns

            Text {
                text: index
                color: "white"
                x: index * grid.blockWidth + 5
            }
        }
    }

    BackButton {}

    Component.onCompleted: {
        anchors.fill = undefined
    }
}
