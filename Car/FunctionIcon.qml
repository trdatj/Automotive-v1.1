import QtQuick 2.15

Item {
    id: icon

    property real iconWidth: 40
    property real iconHeight: 40
    property bool active: true

    property string iconImageOn: ""
    property string iconImageOff: ""
    property bool checked: false
    property bool blinking: false
    property int blinkInterval: 500 // Tốc độ nhấp nháy (ms)

    Image {
        id: iconID
        source: icon.checked ? icon.iconImageOn : icon.iconImageOff
        width: icon.iconWidth
        height: icon.iconHeight
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        visible: icon.active

        transform: Rotation {
            origin.x: icon.width / 2
            origin.y: icon.height / 2
            angle: 0
        }
    }

    onCheckedChanged: {
        console.log("[FunctionIcon.qml] Checked đổi thành", checked)
        console.log("[FunctionIcon.qml] Source đang đổi thành",
                    checked ? iconImageOn : iconImageOff)
        iconID.source = checked ? iconImageOn : iconImageOff
    }
}
