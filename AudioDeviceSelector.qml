import QtQuick

Item {
    id: audioDevice_selector

    height: listview.height
    width: parent.width

    ListView {
        id: listview

        width: parent.width

        height: contentHeight

        x: 30

        model: mediaDevices.audioInputs

        delegate: audioDeviceDelegate

        highlightFollowsCurrentItem: false
    }

    function selectAudioDevice(modelData) {}

    Component {
        id: audioDeviceDelegate

        Item {
            width: parent.width
            height: 30

            Text {
                color: "#ffffff"

                text: modelData.description

                font.pixelSize: 10
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    listview.currentIndex = index
                    highlighter.y = listview.currentItem.y

                    transcription.audioDevice = modelData
                }
            }
        }
    }
    Rectangle {
        id: highlighter

        x: 10

        width: 10
        height: 10
        radius: 5

        color: "white"

        Behavior on y {
            NumberAnimation {
                duration: 100
                easing.type: Easing.InOutQuad
            }
        }
    }
}
