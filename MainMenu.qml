import QtQuick

Rectangle {
    anchors.fill: parent

    color: "#2f2f2f"

    Row {
        Rectangle {
            id: load_song_button

            width: 100
            height: 100
            radius: 10

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    console.log("Pushing song loading screen")
                    stackview.push(song_loading_screen)
                }
            }

            Text {
                anchors.centerIn: parent
                text: "Load song"
            }
        }

        Rectangle {
            id: record_button

            radius: 10
            width: 100
            height: 100

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    console.log("Pushing recording screen")
                    stackview.push(recording_screen)
                }
            }

            Text {
                anchors.centerIn: parent
                text: "Record"
            }
        }
    }

    Component.onCompleted: {
        anchors.fill = null
    }
}
