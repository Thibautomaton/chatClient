import QtQuick 2.0

Item {
    id: message

    height:contentMessage.height + senderName.height + timeInfo.height +20
    //width: parent.ListView.view.width
    //pourrait incorporer des actions notamment sur les actions de zoom, selection, etc.
    required property int listviewWidth;

    required property string content;
    required property string sender;
    required property date time;
    //required property string color;

    width :listviewWidth*3/4;
    required property string colorMessage;


    Rectangle {

        anchors.fill: parent
        color: colorMessage;
        radius: 15
        border.color : "black"

    }

    Text {
        id: senderName
        text: sender
        font.bold: true
        font.family: "Yu Gothic UI Light"

        anchors {
            top: parent.top
            margins: 5
            left: parent.left
        }
    }

    Text {
        id:contentMessage

        width: message.width*2/3

        text : content
        font.family: "Yu Gothic UI Light"
        font.pointSize: 15

        anchors {
            top: senderName.bottom
            left: parent.left
            margins: 5
        }

        z:1
    }

    Text {
        id: timeInfo
        text: time.toLocaleTimeString().replace(/:\d+ /, ' ');
        font.pointSize: 8
        font.italic: true

        anchors {
            bottom: parent.bottom
            right: parent.right
            margins: 3
        }
    }
}
