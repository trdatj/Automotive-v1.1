import QtQuick 2.15

Item {
    id: tempIcon
    width: 100
    height: 45

    property string status: "NORMAL"
    onStatusChanged: {
        console.log("Status changed to:", status)
        state = status.toLowerCase()
    }

    property string normalSource: ""
    property string warningSource: ""
    property string dangerousSource: ""

    //property real temperature: 0 // Thêm property nhiệt độ
    //property string temperatureUnit: "°C" // Đơn vị nhiệt độ
    Image {
        id: iconImage
        anchors.fill: parent
        opacity: 0.9
        source: tempIcon.normalSource

        Behavior on source {
            PropertyAnimation {
                duration: 200
            }
        }
    }

    Text {
        id: tempText
        //text: tempIcon.temperature.toFixed(1) + tempIcon.temperatureUnit
        anchors {
            left: iconImage.right // Neo bên phải icon
            leftMargin: 15 // Khoảng cách với icon
            verticalCenter: parent.verticalCenter
        }
        color: {
            if (tempIcon.status === "DANGEROUS")
                return "red"
            else if (tempIcon.status === "WARNING")
                return "yellow"
            else
                return "white"
        }
        font.pixelSize: 24
        verticalAlignment: Text.AlignVCenter
        height: 45
    }

    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: iconImage
                source: tempIcon.normalSource
            }
        },
        State {
            name: "warning"
            PropertyChanges {
                target: iconImage
                source: tempIcon.warningSource
            }
        },
        State {
            name: "dangerous"
            PropertyChanges {
                target: iconImage
                source: tempIcon.dangerousSource
            }
        }
    ]
}
