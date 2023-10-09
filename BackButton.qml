import QtQuick

Rectangle {
    anchors.top: parent.top
    anchors.right: parent.right

    width: 20
    height: 10

    color: "blue"

    MouseArea {
        anchors.fill: parent

        onClicked: {
            stackview.pop()
        }
    }
}
