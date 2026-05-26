import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 800
    height: 600
    visible: true
    color: "#2c3e50"
    title: "Kral Tuner"

    // Seçili nota state'i
    property int selectedMidi: -1
    property var noteFreqs: ({ 40: 82.41, 45: 110.0, 50: 146.83, 55: 196.0, 59: 246.94, 64: 329.63 })

    Row {
        id: topbar
        anchors.top: parent.top
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 15

        Repeater {
            model: [
                { name: "E", midi: 40 },
                { name: "A", midi: 45 },
                { name: "D", midi: 50 },
                { name: "G", midi: 55 },
                { name: "B", midi: 59 },
                { name: "e", midi: 64 }
            ]

            Item {
                width: 70
                height: 80

                Rectangle {
                    id: noteBtn
                    width: 56
                    height: 56
                    radius: 28
                    anchors.horizontalCenter: parent.horizontalCenter

                    property bool matched: backend.midinum === modelData.midi
                    property bool selected: selectedMidi === modelData.midi
                    property bool active: matched || selected

                    color: matched  ? "#2ecc71"
                         : selected ? "#f39c12"
                         : "#34495e"

                    border.color: matched  ? "#27ae60"
                                : selected ? "#e67e22"
                                : "#2c3e50"
                    border.width: active ? 3 : 2

                    Behavior on color      { ColorAnimation  { duration: 180 } }
                    Behavior on border.color { ColorAnimation { duration: 180 } }

                    // Dış halka (glow)
                    Rectangle {
                        anchors.centerIn: parent
                        width:  parent.width  + 12
                        height: parent.height + 12
                        radius: width / 2
                        color: "transparent"
                        border.color: noteBtn.matched  ? "#2ecc71"
                                    : noteBtn.selected ? "#f39c12"
                                    : "transparent"
                        border.width: 2
                        opacity: noteBtn.active ? 0.55 : 0
                        Behavior on opacity { NumberAnimation { duration: 200 } }
                    }

                    // Pulse animasyonu — sadece eşleşince
                    SequentialAnimation {
                        running: noteBtn.matched
                        loops:   Animation.Infinite
                        NumberAnimation {
                            target: noteBtn; property: "scale"
                            from: 1.1; to: 1.2
                            duration: 420; easing.type: Easing.SineCurve
                        }
                        NumberAnimation {
                            target: noteBtn; property: "scale"
                            from: 1.2; to: 1.1
                            duration: 420; easing.type: Easing.SineCurve
                        }
                    }

                    // Scale (eşleşmeyince sabit)
                    scale: noteBtn.active && !noteBtn.matched ? 1.1 : (noteBtn.matched ? noteBtn.scale : 1.0)

                    // Nota harfi
                    Text {
                        text: modelData.name
                        color: noteBtn.active ? "#1e1e1e" : "white"
                        font.pixelSize: 22
                        font.bold: true
                        anchors.centerIn: parent
                        Behavior on color { ColorAnimation { duration: 180 } }
                    }

                    // Frekans etiketi (içte küçük)
                    Text {
                        text: noteFreqs[modelData.midi] + " Hz"
                        color: noteBtn.active ? "#1e1e1e" : "#7f8c8d"
                        font.pixelSize: 8
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 6
                        anchors.horizontalCenter: parent.horizontalCenter
                        Behavior on color { ColorAnimation { duration: 180 } }
                    }

                    // ✓ rozeti — eşleşince sağ üst köşe
                    Rectangle {
                        id: checkBadge
                        width: 20; height: 20; radius: 10
                        color: "#27ae60"
                        visible: noteBtn.matched
                        anchors.top:   parent.top
                        anchors.right: parent.right
                        anchors.topMargin:   -5
                        anchors.rightMargin: -5
                        z: 5

                        Text {
                            text: "✓"
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                            anchors.centerIn: parent
                        }

                        // Rozet belirince küçükten büyüğe çık
                        scale: noteBtn.matched ? 1.0 : 0.0
                        Behavior on scale {
                            NumberAnimation { duration: 250; easing.type: Easing.OutBack }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            selectedMidi = (selectedMidi === modelData.midi) ? -1 : modelData.midi
                        }
                    }
                }

                // "● dinleniyor" — seçili ama henüz eşleşmemiş
                Text {
                    text: "● dinleniyor"
                    color: "#f39c12"
                    font.pixelSize: 9
                    visible: (selectedMidi === modelData.midi) && (backend.midinum !== modelData.midi)
                    anchors.top: noteBtn.bottom
                    anchors.topMargin: 4
                    anchors.horizontalCenter: parent.horizontalCenter

                    SequentialAnimation on opacity {
                        running: (selectedMidi === modelData.midi) && (backend.midinum !== modelData.midi)
                        loops: Animation.Infinite
                        NumberAnimation { from: 1.0; to: 0.3; duration: 600 }
                        NumberAnimation { from: 0.3; to: 1.0; duration: 600 }
                    }
                }

                // "✔ akort!" — eşleşince alt yazı
                Text {
                    text: "✔ akort!"
                    color: "#2ecc71"
                    font.pixelSize: 10
                    font.bold: true
                    visible: noteBtn.matched
                    anchors.top: noteBtn.bottom
                    anchors.topMargin: 4
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    // Seçili nota bilgi bandı
    Rectangle {
        id: infoBand
        anchors.top: topbar.bottom
        anchors.topMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter
        width: 260
        height: selectedMidi !== -1 ? 32 : 0
        radius: 16
        color: "#f39c12"
        opacity: selectedMidi !== -1 ? 1 : 0
        clip: true

        Behavior on height  { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
        Behavior on opacity { NumberAnimation { duration: 200 } }

        Text {
            anchors.centerIn: parent
            text: {
                var names = { 40:"E", 45:"A", 50:"D", 55:"G", 59:"B", 64:"e" }
                var f = noteFreqs[selectedMidi]
                return names[selectedMidi] + " teline akort ediliyor — " + (f ? f + " Hz" : "")
            }
            color: "#1e1e1e"
            font.pixelSize: 13
            font.bold: true
        }
    }

    Rectangle {
        id: gauger
        width: 400
        height: 250
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 30
        color: "#2c3e50"
        radius: 20
        border.color: "#34495e"
        border.width: 4

        Repeater {
            model: [-50, -25, 0, 25, 50]
            Item {
                width: 2
                height: 170
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 30
                anchors.horizontalCenter: parent.horizontalCenter
                transformOrigin: Item.Bottom
                rotation: modelData

                Rectangle {
                    width: modelData === 0 ? 5 : 3
                    height: modelData === 0 ? 16 : 10
                    color: modelData === 0 ? "#2ecc71" : "#95a5a6"
                    radius: 2
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: modelData === 0 ? "0" : (modelData > 0 ? "+" + modelData : modelData)
                    color: modelData === 0 ? "#2ecc71" : "#95a5a6"
                    font.pixelSize: 13
                    font.bold: true
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        // İbre (needle)
        Rectangle {
            id: ibre
            width: 6
            height: 140
            radius: 3
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            transformOrigin: Item.Bottom
            rotation: backend.cents
            color: Math.abs(backend.cents) <= 3 ? "#2ecc71" : "#e74c3c"

            // İbre başı (üst yuvarlak nokta)
            Rectangle {
                width: 12
                height: 12
                radius: 6
                color: parent.color
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: -3
                Behavior on color { ColorAnimation { duration: 200 } }
            }

            Behavior on rotation {
                NumberAnimation { duration: 500; easing.type: Easing.OutBack }
            }
            Behavior on color { ColorAnimation { duration: 200 } }
        }

        // Merkez pivot noktası
        Rectangle {
            width: 14
            height: 14
            radius: 7
            color: "#ecf0f1"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 23
            anchors.horizontalCenter: parent.horizontalCenter
            z: 10
        }

        Text {
            id: notename
            text: backend.note
            color: "#f1c40f"
            font.pixelSize: 40
            font.bold: true
            anchors.top: parent.bottom
            anchors.topMargin: 16
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: freq
            text: backend.freq ? backend.freq.toFixed(2) + " Hz" : "-- Hz"
            color: "#bdc3c7"
            font.pixelSize: 22
            anchors.top: notename.bottom
            anchors.topMargin: 4
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Cent göstergesi yazısı
        Text {
            text: (backend.cents >= 0 ? "+" : "") + backend.cents.toFixed(1) + " cent"
            color: Math.abs(backend.cents) <= 3 ? "#2ecc71" : "#e74c3c"
            font.pixelSize: 16
            font.bold: true
            anchors.top: freq.bottom
            anchors.topMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
            Behavior on color { ColorAnimation { duration: 200 } }
        }
    }

}
