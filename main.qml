import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.3

/**
  status correspondance :
  notConnected -> 256
  notloggedin -> 257
  loggedin-> 258
  **/


ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("chat client")

    property string currentUser:"anonyme"

    Rectangle {
        id: informationRectangle
        anchors {
            bottom: parent.bottom
            right: parent.right

        }
        height: 200
        width: 200
        color: "white"
        border.color: "black"
        border.width: 3

        Text {
            id: informationPanel
            text : "informations"
            padding: 10
        }
    }


    Rectangle {
        id: loginPanel

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
        }
        color: "lightgrey"
        width: 150
        height: 120
        Column {
            anchors.fill: parent
            spacing: 7
            Text {
                text : "login panel :"
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
            }

            TextInput {
                id:username
                text: "your username"
                padding: 5
                anchors.horizontalCenter: parent.horizontalCenter
                font.italic: true
                font.pointSize: 8
                color: "white"

                z:1
                Rectangle {
                    anchors.fill: parent
                    z:0
                    color: "grey"
                    opacity: 0.2
                }

                Connections {
                    target:username
                    onEditingFinished:{currentUser = username.text}
                }
            }

            TextInput {
                id: passwdInput
                padding:5
                text: "password"
                anchors.horizontalCenter: parent.horizontalCenter
                font.italic: true
                font.pointSize: 8
                color: "white"

                Rectangle {
                    anchors.fill: parent
                    z:0
                    color: "grey"
                    opacity: 0.2
                }
            }

            Button {
                id: loginButton
                text: "login"
                anchors.horizontalCenter: parent.horizontalCenter
                width: 50
                height: 25

                onClicked: {
                    if(ChatClient.getCurrentStatus()==256)
                    {
                        //opendialog to inform that you must connect first
                    }
                    else if(ChatClient.getCurrentStatus()==257)
                    {
                        ChatClient.attemptLogin(username.text, passwdInput.text);
                        //open dialog if successful
                    }
                    else if(ChatClient.getCurrentStatus()==258)
                    {
                        //open dialog to inform that client is already connected
                    }
                }
            }
        }
    }

    Rectangle {
        id: connexionPanel

        width: parent.width/4>165 ? parent.width/4 : 165
        height: 170

        color: "grey"

        anchors {
            right: parent.right
            top: parent.top
        }

        Column {
            anchors.fill: parent

            spacing: 10

            Text {
                id: titleConnexion
                text: "Connect to server:"
                color: "white"
                font.family: "Courier"
                anchors {
                    left: parent.left
                    //top: parent.top
                    margins: 10
                    horizontalCenter: parent.horizontalCenter
                }
            }

            TextInput
            {
                id: ipInput
                text: "127.0.0.1"
                font.pointSize: 15
                font.family: "Courier"
                font.italic: true
                opacity: 0.7
                padding: 5

                anchors {
                    left: parent.left
                    margins: 10
                    //top: titleConnexion.bottom
                    horizontalCenter: parent.horizontalCenter
                }
                z:1
                Rectangle {
                    id: ipbackground
                    anchors.fill: parent
                    color: "lightgrey"
                    z:0
                }
            }

            TextInput
            {
                id: portInput
                text: "1967"
                font.pointSize: 15
                font.family: "Courier"
                font.italic: true
                opacity: 0.7
                padding: 5

                anchors {
                    left: parent.left
                    margins: 10
                    //top: ipInput.bottom
                    horizontalCenter: parent.horizontalCenter
                }
                z:1
                Rectangle {
                    id: portBackground
                    anchors.fill: parent
                    color: "lightgrey"
                    z:0
                }
            }

            Button {
                text: "connect"
                font.family: "Courier"
                font.pointSize: 25
                padding: 5

                anchors {
                    horizontalCenter: parent.horizontalCenter
                    //bottom: parent.bottom
                    margins: 7
                    //bottom: parent.bottom
                }

                onClicked: {
                    //need to convert elements inside the function
                    //we get strings-> need to convert again to numbers
                    ChatClient.attemptConnection(ipInput.text, portInput.text);
                    console.log("clicked");
                }
            }
        }


    }

    Row {
        anchors {
            top: parent.top
            left: parent.left
            margins: 50
        }
        spacing: 20

        Column {
            Rectangle {
                id: connexionStatusC

                color: "lightgreen"
                opacity: 0.3
                radius: 100

                border.color: "grey"
                border.width: 1

                width: 30
                height: 30
            }

            Text {
                text: "connected \nand\nlogged in"
                font.italic: true
                font.family: "Times New Roman"
                font.pointSize: 6
            }
        }
        Column {
            Rectangle {
                id: connexionStatusNC

                color: "orange"
                opacity: 0.3
                radius: 100

                border.color: "grey"
                border.width: 1


                width: 30
                height: 30
            }
            Text {
                text: "connected \nbut not\nyet logged in"
                font.italic: true
                font.family: "Times New Roman"
                font.pointSize: 6
            }
        }

        Column {
            Rectangle {
                id: connexionStatusD

                color: "red"
                opacity: 0.3
                radius: 100

                border.color: "grey"
                border.width: 1


                width: 30
                height: 30
            }
            Text {
                text: "not \nconnected"
                font.family: "Times New Roman"
                font.italic: true
                font.pointSize: 6
            }
        }
    }


    Rectangle {
        id: areaMessages

        anchors {
            margins: 10
            left: parent.left
            bottom: parent.bottom
            top: loginPanel.bottom
        }

        width: parent.width*2/3 > 150 ? parent.width*2/3 : 150
        color: "lightgray"




        ListView {
            id: messagesDisplay
            anchors.fill: parent
            highlightRangeMode: ListView.StrictlyEnforceRange
            clip:true
            spacing : 5
            footerPositioning: ListView.OverlayFooter

            Component {
                id: messageComponent
                Message {
                    id: message
                    content: messagesModel.get(index).contentValue;
                    sender: messagesModel.get(index).senderValue;
                    time: new Date();
                    listviewWidth: messagesDisplay.width;
                    colorMessage : sender ==currentUser ? "lightgray" : "lightblue ";
                    anchors {
                        left : sender == currentUser ? " " : parent.left
                        right : sender == currentUser ? parent.right : " "
                    }

                    required property int index;
                    z:1
                }
            }


            model:  ListModel {
                id: messagesModel
//                ListElement {
//                    contentValue: "new Message"
//                    senderValue: "new user"
//                }
//                ListElement {
//                    contentValue: "second element"
//                    senderValue: "second user"
//                }
            }


            delegate : messageComponent

            footer : Rectangle {
                height:100
                color: "grey"
                border.color: "lightgrey"
                border.width: 5
                width: messagesDisplay.width
                z:50

                Rectangle {
                    anchors {
                        top: parent.top
                        left: parent.left
                        margins:15

                    }
                    width: parent.width*2/3
                    height: parent.height -30
                    color: "lightgrey"
                    radius: 10

                    TextInput {
                        id: messageInput
                        anchors.fill: parent
                        text: "tap your message..."
                        font.family: "Courier"
                        font.italic: true
                        opacity: 0.5
                        anchors {
                            top: parent.top
                            left: parent.left
                            margins:5
                        }
                    }

                }
                Button {
                    text: "send Message"
                    anchors
                    {
                        top: parent.top
                        right: parent.right
                        margins: 25


                    }
                    focus:true

                    onClicked: {
                        //we should send the text value in the textInput item to the server so that it constitutes a new message of it
                        ChatClient.sendMessage(messageInput.text);

                        messagesModel.append({contentValue:messageInput.text, senderValue:currentUser});
                    }
                }
            }
        }
    }


    Connections {
        target: ChatClient
        function onConnected() {
            connexionStatusNC.opacity =1;
            connexionStatusD.opacity = 0.3;
            connexionStatusC.opacity = 0.3;
        }
        function onLoggedIn() {
            connexionStatusC.opacity = 1;
            connexionStatusD.opacity = 0.3;
            connexionStatusNC.opacity = 0.3;
            informationPanel.text = "successfully connected !"
        }
        function onLoginError(reason) {
            informationPanel.text = "couldn't connect because of :\n" + reason;
            console.log("okk");
        }

        function onUserJoined(userName) {
            //add single line message saying user joined
        }

        function onMessageReceived(user, text)
        {
            messagesModel.append({contentValue:text, senderValue:user});
        }


//        function messageReceived(sender, text) {
//            messagesModel.append(ListElement{
//                                     )
//        }
    }
}
