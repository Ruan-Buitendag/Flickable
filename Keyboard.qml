import QtQuick

Item {
    id: keyboard

    property int num_octaves: 7

    //    property int totalheight: octaves.height
    height: octaves.height
    width: octaves.width

    Rectangle {

        color: "#2f2f2f"
        height: octaves.height
        width: octaves.width - 5

        Column {
            id: octaves

            spacing: 4

            KeyboardNote {
                noteName: "C"
                noteOctave: num_octaves
            }

            Repeater {
                model: num_octaves

                Octave {
                    octaveNumber: num_octaves - index - 1
                }
            }
        }
    }
}
