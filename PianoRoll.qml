import QtQuick
import Transcription

Rectangle {
    id: screen_background

    anchors.fill: parent

    color: "#2f2f2f"

    Rectangle {
        id: pianoroll_background

        width: parent.width
        height: parent.height

        x: 0

        color: "#2f2f2f"

        Flickable {

            x: keyboard.width

            id: note_flickable

            width: parent.width - keyboard.width
            height: parent.height

            property int totalwidth: parent.width

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

                Rectangle {
                    id: playback_line

                    width: 1

                    color: "green"

                    height: flickable_background.height

                    NumberAnimation on x {
                        id: playback_line_animation

                        to: note_flickable.contentWidth
                        duration: note_flickable.contentWidth / grid.blockWidth * 1000
                        easing.type: Easing.Linear
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

        Component.onCompleted: {
            anchors.fill = undefined
        }

        Behavior on x {
            NumberAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    }

    Rectangle {
        id: nav_bar

        color: "#2f2f2f"

        height: parent.height
        width: back_button.width

        x: -width

        Behavior on x {
            NumberAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    }

    MouseArea {
        height: parent.height
        width: 200
        x: 0

        hoverEnabled: true

        onEntered: {
            pianoroll_background.x = nav_bar.width
            nav_bar.x = 0
            back_button.x = 0
        }
        onExited: {
            pianoroll_background.x = 0
            nav_bar.x = -nav_bar.width
            back_button.x = -nav_bar.width
        }
    }

    BackButton {
        id: back_button
        x: -width

        Behavior on x {
            NumberAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    }

    Rectangle {
        id: play_button

        anchors.right: parent.right
        anchors.top: parent.top

        width: 25
        height: 25

        color: "white"

        MouseArea {
            anchors.fill: parent

            onClicked: {
                transcription.playMidiFile()
                playback_line.x = 0
                playback_line_animation.restart()
            }
        }
    }

    Component.onCompleted: anchors.fill = undefined
}
