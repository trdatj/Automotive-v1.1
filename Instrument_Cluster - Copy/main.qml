import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: root
    width: 1920
    height: 980
    visible: true
    title: qsTr("Hello World")
    property bool showMainContent: false

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
                            if (turnLeftID.blinking) {
                                turnLeftID.blinking = false
                                turnLeftID.checked = false
                                console.log("Left turn signal: OFF")
                                serialManager.sendData("TURN_LEFT:OFF")
                            } else {
                                turnLeftID.blinking = true
                                turnLeftID.checked = true
                                console.log("Left turn signal: ON")
                                serialManager.sendData("TURN_LEFT:ON")
                            }
                            if (turnLeftID.blinking) {
                                turnRightID.blinking = false
                                turnRightID.checked = false
                            }
                        }
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

                            if (turnRightID.blinking) {
                                turnRightID.blinking = false
                                turnRightID.checked = false
                                console.log("Right turn signal: OFF")
                                serialManager.sendData("TURN_RIGHT:OFF")
                            } else {
                                turnRightID.blinking = true
                                turnRightID.checked = true
                                console.log("Right turn signal: ON")
                                serialManager.sendData("TURN_RIGHT:ON")
                            }

                            if (turnRightID.blinking) {
                                turnLeftID.blinking = false
                                turnLeftID.checked = false
                            }
                        }
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
                    anchors.leftMargin: 70
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
                    anchors.rightMargin: 70
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
                            hazardBlinkTimer.start()

                            hazardID.wasLeftBlinkingBeforeHazard = turnLeftID.blinking
                            hazardID.wasRightBlinkingBeforeHazard = turnRightID.blinking

                            turnLeftID.blinking = false
                            turnLeftID.checked = false
                            turnRightID.blinking = false
                            turnRightID.checked = false

                            console.log("Hazard: ON")
                            serialManager.sendData("HAZARD:ON")
                        } else {
                            hazardBlinkTimer.stop()
                            turnLeftID.blinking = false
                            turnLeftID.checked = false
                            turnRightID.blinking = false
                            turnRightID.checked = false

                            console.log("Hazard: OFF")
                            serialManager.sendData("HAZARD:OFF")

                            // Khôi phục trạng thái nhấp nháy của xi nhan trước đó
                            if (hazardID.wasLeftBlinkingBeforeHazard) {
                                turnLeftID.blinking = true
                                turnLeftID.checked = true
                                serialManager.sendData("TURN_LEFT:ON")
                            }
                            if (hazardID.wasRightBlinkingBeforeHazard) {
                                turnRightID.blinking = true
                                turnRightID.checked = true
                                serialManager.sendData("TURN_RIGHT:ON")
                            }

                            // Đặt lại cờ lưu trạng thái
                            hazardID.wasLeftBlinkingBeforeHazard = false
                            hazardID.wasRightBlinkingBeforeHazard = false
                        }
                    }
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

    Connections {
        target: serialManager

        function onSignalChanged(message) {
            console.log("NHẬN SIGNAL RAW:", JSON.stringify(message))

            const cleanMsg = message.trim()
            console.log("SAU TRIM:", JSON.stringify(cleanMsg))

            const parts = cleanMsg.split(":")
            if (parts.length !== 2) {
                console.log("MESSAGE KHÔNG HỢP LỆ:", cleanMsg)
                return
            }

            const device = parts[0]
            const status = parts[1]

            switch (device) {
            case "TURN_LEFT":
                if (status === "ON") {
                    console.log("Xi nhan trái bật")
                    turnLeftID.blinking = true
                    turnLeftID.checked = true
                } else {
                    console.log("Xi nhan trái tắt")
                    turnLeftID.blinking = false
                    turnLeftID.checked = false
                }
                break
            case "TURN_RIGHT":
                if (status === "ON") {
                    console.log("Xi nhan phải bật")
                    turnRightID.blinking = true
                    turnRightID.checked = true
                } else {
                    console.log("Xi nhan phải tắt")
                    turnRightID.blinking = false
                    turnRightID.checked = false
                }
                break
            case "HAZARD":
                if (status === "ON") {
                    console.log("Đèn hazard bật")
                    hazardID.checked = true // Bật icon
                    turnRightID.blinking = true
                    turnRightID.checked = true
                    turnLeftID.blinking = true
                    turnLeftID.checked = true
                } else {
                    console.log("Đèn hazard tắt")
                    hazardID.checked = false // Tắt icon
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
                var nonLinearProgress = Math.pow(normalizedPotValue, 2.0)

                var mappedRpmAngle = minRpmAngle + nonLinearProgress * (maxRpmAngle - minRpmAngle)
                speedometerRight.angle = mappedRpmAngle
                break
            default:
                console.log("Invalid Device:", device)
                break
            }
        }
    }
}
