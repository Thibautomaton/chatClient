#include "infodisplay.h"

InfoDisplay::InfoDisplay(QObject *parent) : QObject(parent)
{
    connect(client, &ChatClient::messageReceived, this, &InfoDisplay::newMessage);
}

void InfoDisplay::newMessage(QString senderVal, QString textVal)
{
    Message newMessage(textVal, senderVal);

    m_messages.append(newMessage);
}
