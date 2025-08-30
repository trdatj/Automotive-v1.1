import QtQuick 2.15
import QtQuick.Window 2.15
import com.mycompany.network 1.0

Window {
    id: root
    width: 1920
    height: 980
    visible: true
    title: qsTr("Instrument Cluster")
    property bool showMainContent: false

    property string currentSignSource: ""

    Item {
        id: content
        width: 1920
        height: 980

        anchors.centerIn: parent
        scale: Math.min(root.width / 1920,
                        root.height / 980) // scale toàn bộ nội dung

        Behavior on opacity {
            NumberAnimation {
                duration: 500
                easing.type: Easing.InOutQuad
            }
        }

        //Background
        Image {
            id: background
            source: "qrc:/img/Panel.png"
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            property int currentAngle: 150

            //Mặt đồng hồ trái
            Speedometer {
                id: speedometerLeft
                width: 470
                height: 470
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 150

                meterImage: "qrc:/img/speedImg.png"
                indicatorImage: "qrc:/img/Indicator.png"
                angle: 148
            }

            //Mặt đồng hồ phải
            Speedometer {
                id: speedometerRight
                width: 470
                height: 470
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 150

                meterImage: "qrc:/img/tachoImg.png"
                indicatorImage: "qrc:/img/Indicator.png"
                angle: 150
            }

            //----car----
            // Image {
            //     id: r34
            //     source: "qrc:/img/nissan_skyline_gtr.png"
            //     width: 725
            //     height: 339
            //     anchors.horizontalCenter: parent.horizontalCenter
            //     anchors.centerIn: parent
            // }
            Image {
                id: lane
                source: "qrc:/icons/Road/road.png"
                width: 725
                height: 390
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.centerIn: parent

                Image {
                    id: car
                    source: "qrc:/icons/Road/car.png"
                    //anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 250
                }
            }

            //----top bar----
            Image {
                id: topbarID
                width: 960
                height: 110
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 80
                source: "qrc:/img/top.png"

                //logo
                Image {
                    id: logoID
                    source: "qrc:/icons/nissan-6.svg"
                    width: 100
                    height: 85
                    anchors.centerIn: topbarID
                }

                //Nhiệt độ xe
                TempIcon {
                    id: cabinTempID
                    width: 45
                    height: 45
                    opacity: 0.9
                    anchors.verticalCenter: topbarID.verticalCenter
                    anchors.left: topbarID.left
                    anchors.leftMargin: 100

                    status: "NORMAL"
                    normalSource: "qrc:/icons/icons-left/temp_cabin.svg"
                    warningSource: "qrc:/icons/icons-left-checked/temp_cabin_warning.svg"
                    dangerousSource: "qrc:/icons/icons-left-checked/temp_cabin_danger.svg"

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 300
                        }
                    }
                }

                //----nhiệt độ nước mát----
                TempIcon {
                    id: oilTempID
                    width: 45
                    height: 45
                    opacity: 0.9
                    anchors.right: topbarID.right
                    anchors.rightMargin: 165
                    anchors.verticalCenter: topbarID.verticalCenter

                    normalSource: "qrc:/icons/icons-right/temp_oil.svg"
                    warningSource: "qrc:/icons/icons-right-checked/temp_oil_warning.svg"
                    dangerousSource: "qrc:/icons/icons-right-checked/temp_oil_danger.svg"

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 300
                        }
                    }
                }

                //----xi nhan trái----
                FuncIcon {
                    id: turnLeftID
                    width: 100
                    height: 65
                    anchors.left: topbarID.left
                    anchors.verticalCenter: topbarID.verticalCenter
                    anchors.leftMargin: -70
                    checked: false

                    iconImageOff: "qrc:/icons/icons-left/xi_nhan_trai.svg"
                    iconImageOn: "qrc:/icons/icons-left-checked/xi_nhan_trai_checked.svg"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            turnLeftID.checked = !turnLeftID.checked

                            if (turnLeftID.checked) {
                                turnLeftID.blinking = true
                                turnLeftID.checked = true
                                turnRightID.blinking = false
                                turnRightID.checked = false
                                console.log("Left turn signal ON")
                                serialManager.sendData("TURN_LEFT:ON")
                            } else {
                                turnLeftID.blinking = false
                                turnLeftID.checked = false
                                console.log("Left turn signal: OFF")
                                serialManager.sendData("TURN_LEFT:OFF")
                            }
                        }
                    }

                    Timer {
                        id: blinkTimerLeft
                        interval: turnLeftID.blinkInterval
                        running: turnLeftID.blinking
                        repeat: true
                        onTriggered: turnLeftID.checked = !turnLeftID.checked
                    }
                }

                //----xi nhan phải----
                FuncIcon {
                    id: turnRightID
                    width: 100
                    height: 65
                    anchors.right: topbarID.right
                    anchors.verticalCenter: topbarID.verticalCenter
                    anchors.rightMargin: -70
                    checked: false

                    iconImageOff: "qrc:/icons/icons-right/xi_nhan_phai.svg"
                    iconImageOn: "qrc:/icons/icons-right-checked/xi_nhan_phai_checked.svg"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            turnRightID.checked = !turnRightID.checked

                            if (turnRightID.checked) {
                                turnRightID.checked = true
                                turnRightID.blinking = true
                                turnLeftID.checked = false
                                turnLeftID.blinking = false
                                console.log("Right turn signal ON")
                                serialManager.sendData("TURN_RIGHT:ON")
                            } else {
                                turnRightID.blinking = false
                                turnRightID.checked = false
                                console.log("Right turn signal OFF")
                                serialManager.sendData("TURN_RIGHT:OFF")
                            }
                        }
                    }

                    Timer {
                        id: blinkTimerRight
                        interval: turnRightID.blinkInterval
                        running: turnRightID.blinking
                        repeat: true
                        onTriggered: turnRightID.checked = !turnRightID.checked
                    }
                }
            }

            //----bottom bar----
            Image {
                id: bottomBarID
                source: "qrc:/img/bottom.png"
                width: 960
                height: 110
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 80

                //----Thời gian----
                Text {
                    id: realTimeID
                    anchors.verticalCenter: bottomBarID.verticalCenter
                    anchors.horizontalCenter: bottomBarID.horizontalCenter
                    color: "white"
                    font.pixelSize: 24
                    text: Qt.formatDateTime(new Date(), "hh:mm:ss AP")
                }

                Timer {
                    id: realTimeUpdateID
                    interval: 1000
                    running: true
                    repeat: true
                    onTriggered: {
                        realTimeID.text = Qt.formatDateTime(new Date(),
                                                            "hh:mm:ss AP")
                    }
                }

                //----cos----
                FuncIcon {
                    id: cosID
                    width: 60
                    height: 60
                    opacity: 0.9
                    anchors.verticalCenter: bottomBarID.verticalCenter
                    anchors.left: bottomBarID.left
                    anchors.leftMargin: 100
                    checked: false

                    iconImageOn: "qrc:/icons/icons-left-checked/light_cos_checked.svg"
                    iconImageOff: "qrc:/icons/icons-left/light_cos.svg"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (cosID.checked) {
                                cosID.checked = false
                                console.log("COS: OFF")
                            } else {
                                cosID.checked = true
                                console.log("COS: ON")
                            }
                        }
                    }
                }

                //----pha----
                FuncIcon {
                    id: phaID
                    width: 60
                    height: 60
                    opacity: 0.9
                    anchors.verticalCenter: bottomBarID.verticalCenter
                    anchors.right: bottomBarID.right
                    anchors.rightMargin: 100
                    checked: false

                    iconImageOn: "qrc:/icons/icons-left-checked/light-high-checked.svg"
                    iconImageOff: "qrc:/icons/icons-left/light_high.svg"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (phaID.checked) {
                                phaID.checked = false
                                console.log("PHA: OFF")
                            } else {
                                phaID.checked = true
                                console.log("PHA: ON")
                            }
                        }
                    }
                }
            }

            //----Hazard----
            FuncIcon {
                id: hazardID
                width: 60
                height: 60
                opacity: 0.9
                checked: false
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: topbarID.top
                anchors.topMargin: 135

                iconImageOn: "qrc:/icons/icons-left-checked/hazard_light_checked.png"
                iconImageOff: "qrc:/icons/icons-left/hazard_light.svg"

                property bool wasLeftBlinkingBeforeHazard: false
                property bool wasRightBlinkingBeforeHazard: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        hazardID.checked = !hazardID.checked

                        if (hazardID.checked) {
                            console.log("Hazard on")
                            hazardBlinkTimer.start()

                            hazardID.wasLeftBlinkingBeforeHazard = turnLeftID.blinking
                            hazardID.wasRightBlinkingBeforeHazard = turnRightID.blinking

                            if (hazardID.wasLeftBlinkingBeforeHazard) {
                                serialManager.sendData("TURN_LEFT:OFF")
                                turnLeftID.blinking = false
                                turnLeftID.checked = false
                                blinkTimerLeft.stop()
                            }
                            if (hazardID.wasRightBlinkingBeforeHazard) {
                                serialManager.sendData("TURN_RIGHT:OFF")
                                turnRightID.blinking = false
                                turnRightID.checked = false
                                blinkTimerRight.stop()
                            }
                            serialManager.sendData("HAZARD:ON")
                        } else {
                            hazardBlinkTimer.stop()
                            hazardID.checked = false
                            blinkTimerLeft.stop()
                            blinkTimerRight.stop()
                            turnRightID.blinking = false
                            turnRightID.checked = false
                            turnLeftID.blinking = false
                            turnLeftID.checked = false
                            console.log("Hazard off")
                            serialManager.sendData("HAZARD:OFF")

                            if (hazardID.wasLeftBlinkingBeforeHazard) {
                                turnLeftID.blinking = true
                                turnLeftID.checked = true
                                blinkTimerLeft.start()
                                serialManager.sendData("TURN_LEFT:ON")
                            } else {
                                turnLeftID.blinking = false
                                turnLeftID.checked = false
                                blinkTimerLeft.stop()
                            }

                            if (hazardID.wasRightBlinkingBeforeHazard) {
                                turnRightID.blinking = true
                                turnRightID.checked = true
                                blinkTimerRight.start()
                                serialManager.sendData("TURN_RIGHT:ON")
                            } else {
                                turnRightID.blinking = false
                                turnRightID.checked = false
                                blinkTimerRight.stop()
                            }

                            hazardID.wasLeftBlinkingBeforeHazard = false
                            hazardID.wasRightBlinkingBeforeHazard = false
                        }
                    }
                }

                Timer {
                    id: hazardBlinkTimer
                    interval: 500
                    running: false
                    repeat: true
                    onTriggered: {
                        turnLeftID.checked = !turnLeftID.checked
                        turnRightID.checked = !turnRightID.checked
                    }
                }
            }

            //---warning----
            Image {
                id: speedID
                source: "qrc:/icons/Road/Frame 33.png"
                width: 120
                height: 110
                opacity: 0.9
                anchors.top: topbarID.top
                anchors.topMargin: 135
                anchors.left: hazardID.left
                anchors.leftMargin: 295

                // property string currentSpeedLimit: "---"

                // Text {
                //     id: speedLimitDisplay
                //     text: "---"
                //     font.pixelSize: 55
                //     color: "black"
                //     anchors.centerIn: parent
                //     horizontalAlignment: Text.AlignHCenter
                //     anchors.top: parent.top
                //     anchors.topMargin: 75
                // }
                Image {
                    id: speedLimitImage
                    source: root.currentSignSource
                    width: 60
                    height: 60
                    anchors.top: parent.top
                    anchors.topMargin: 50
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        StartWindows {
            id: startWindow
            anchors.fill: parent
            active: !root.showMainContent

            onStartClicked: {
                root.showMainContent = true
                console.log("Starting...")
            }
        }

        // Ẩn content ban đầu
        Component.onCompleted: {
            showMainContent = false
        }
    }

    Connections {
        target: serialManager

        function onSignalChanged(message) {
            console.log("NHẬN SIGNAL RAW:", JSON.stringify(message))

            const cleanMsg = message.trim()
            console.log("SAU TRIM:", JSON.stringify(cleanMsg))

            const parts = cleanMsg.split(":")
            if (parts.length !== 2) {
                console.log("INVALID MESSAGE:", cleanMsg)
                return
            }

            const device = parts[0]
            const status = parts[1]

            switch (device) {
            case "TURN_LEFT":
                if (status === "ON") {
                    console.log("Left turn signal on")
                    turnLeftID.blinking = true
                    turnLeftID.checked = true
                } else {
                    console.log("Left turn signal off")
                    turnLeftID.blinking = false
                    turnLeftID.checked = false
                }
                break
            case "TURN_RIGHT":
                if (status === "ON") {
                    console.log("Right turn signal on")
                    turnRightID.blinking = true
                    turnRightID.checked = true
                } else {
                    console.log("Right turn signal off")
                    turnRightID.blinking = false
                    turnRightID.checked = false
                }
                break
            case "HAZARD":
                if (status === "ON") {
                    console.log("Đèn hazard bật")
                    turnRightID.blinking = true
                    turnRightID.checked = true
                    turnLeftID.blinking = true
                    turnLeftID.checked = true
                } else {
                    console.log("Đèn hazard tắt")
                    turnRightID.blinking = false
                    turnRightID.checked = false
                    turnLeftID.blinking = false
                    turnLeftID.checked = false
                }
                break
            case "POT_VAL":
                // Chuyển đổi giá trị từ chuỗi thành số nguyên
                var potValue = parseInt(status)
                if (isNaN(potValue)) {
                    // Kiểm tra nếu giá trị không phải số
                    console.log("POT_VAL không phải số: " + value)
                    break
                }

                var minAngle = 148
                var maxAngle = 392

                var mappedAngle = minAngle + (potValue / 4095.0) * (maxAngle - minAngle)
                speedometerLeft.angle = mappedAngle

                // speedometerRight biểu diễn RPM
                var minRpmAngle = 148
                var maxRpmAngle = 392

                var normalizedPotValue = potValue / 4095.0
                var nonLinearProgress = Math.pow(normalizedPotValue, 1.5)

                var mappedRpmAngle = minRpmAngle + nonLinearProgress * (maxRpmAngle - minRpmAngle)
                speedometerRight.angle = mappedRpmAngle
                break
            default:
                console.log("Invalid Device:", device)
                break
            }
        }
    }

    // NetworkManager {
    //     id: networkManager
    //     onDataReceived: data => {
    //                         var lines = data.split('\n')
    //                         for (var i = 0; i < lines.length; i++) {
    //                             var line = lines[i].trim()
    //                             if (line.startsWith("SPEED_LIMIT:")) {
    //                                 var value = line.split(':')[1]
    //                                 if (value === "NO_SIGN") {
    //                                     speedLimitDisplay.text = "---"
    //                                 } else {
    //                                     speedLimitDisplay.text = value
    //                                 }
    //                             }
    //                         }
    //                     }
    // }
    NetworkManager {
        id: networkManager
        onDataReceived: data => {
                            var lines = data.split('\n')
                            for (var i = 0; i < lines.length; i++) {
                                var line = lines[i].trim()
                                if (line.startsWith("SPEED_LIMIT:")) {
                                    var value = line.split(':')[1]
                                    console.log("Received speed limit value:",
                                                value)
                                    switch (value) {
                                        case "40":
                                        root.currentSignSource = "qrc:/icons/Road/toi_da_40.png"
                                        break
                                        case "50":
                                        root.currentSignSource = "qrc:/icons/Road/toi_da_50.png"
                                        break
                                        case "60":
                                        root.currentSignSource = "qrc:/icons/Road/toi_da_60.png"
                                        break
                                    }
                                }
                            }
                        }
    }

    Component.onCompleted: {
        networkManager.startServer()
    }
}
