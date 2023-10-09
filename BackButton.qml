import QtQuick

//Rectangle {
//    anchors.top: parent.top
//    anchors.right: parent.right

//    width: 20
//    height: 10

//    color: "blue"

//    MouseArea {
//        anchors.fill: parent

//        onClicked: {
//            stackview.pop()
//        }
//    }
//}
Item {

    width: image.width + 20
    height: image.height + 20

    Image {
        anchors.centerIn: parent

        id: image

        source: "images/buttons/back/white.png"

        //        x: 10
        //        y: x
        width: 50
        height: width

        MouseArea {
            anchors.fill: parent

            onClicked: {
                stackview.pop()
            }
        }
    }
}
