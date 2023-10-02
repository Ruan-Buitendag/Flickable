import QtQuick

Item {

    anchors.fill: parent

    property int blockWidth: 50
    property int blockHeight: 24

    property int columns: 0
    property int rows: 0

    Repeater {
        model: columns * rows
        GridBlock {

            x: index % columns * blockWidth
            y: Math.floor(index / columns) * blockHeight - 2

            width: parent.blockWidth
            height: parent.blockHeight
        }
    }
}
