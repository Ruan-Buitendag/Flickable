import QtQuick

Item {
    id: triangle

    property color color: "white"

    x: 50
    y: 50

    Rectangle {
        id: rect

        width: 100
        height: 100

        color: triangle.color

        x: parent.x
        y: parent.y

        // Use the Rotation element to rotate the rectangle to form a triangle
        transform: Rotation {
            origin.x: rect.x + rect.width / 2
            origin.y: rect.y + rect.height / 2
            angle: 45
        }
    }

    Rectangle {
        id: blocker

        height: Math.sqrt(rect.width ** 2 + rect.height ** 2)
        width: height / 2

        x: rect.x
        y: rect.y

        //        color: "#2f2f2f"
        color: "blue"
    }

    //        Rectangle {
    //            width: parent.width / 2
    //            height: 100
    //            color: "white"
    //            visible: true
    //            transform: Rotation {
    //                origin.x: parent.width / 2
    //                origin.y: rect.height
    //                angle: -90
    //            }
    //        }
}
