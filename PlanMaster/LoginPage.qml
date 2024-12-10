import QtQuick 2.15

Item {
    Rectangle {
        id: loginPage
        width: 400
        height: 300
        radius: 10
        color: "white"

        Text {
            text: "Login"
            font.pixelSize: 24
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 20
        }

        TextField {
            id: username
            placeholderText: "Username"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 60
            width: 300
        }

        TextField {
            id: password
            placeholderText: "Password"
            echoMode: TextInput.Password
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: username.bottom
            anchors.topMargin: 20
            width: 300
        }

        Button {
            text: "Login"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: password.bottom
            anchors.topMargin: 20

            onClicked: {
                // Logika logowania
                if (username.text === "admin" && password.text === "1234") {
                    mainStackView.currentIndex = 1 // Przechodzi na dashboard
                } else {
                    console.log("Invalid credentials")
                }
            }
        }
    }

}
