#include "chatclient.h"
#include <QAbstractSocket>
#include <QHostAddress>
#include <QString>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDataStream>
#include <QTcpServer>
#include <QTcpSocket>

ChatClient::ChatClient( QObject *parent) : QObject(parent)
  ,m_clientSocket(new QTcpSocket(this))
  ,m_loggedIn(false)
{
    //redispatch signals. The signals received from the connection to the server

    connect(m_clientSocket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);

    connect(m_clientSocket, &QTcpSocket::connected, this, &ChatClient::connected);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &ChatClient::disconnected);

    //forwarding the error message
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ChatClient::error);

    //reset the m_loggedIn varaible when we disconnect

    connect(m_clientSocket, &QTcpSocket::disconnected, this, [this]()->void{m_loggedIn = false;});

    connect(this, &ChatClient::connected, [this]() {
        m_currentStatus = notloggedin;
    });
    connect(this, &ChatClient::disconnected, [this]() {
        m_currentStatus = notConnected;
    });
}

void ChatClient::attemptConnection(QString ip_address, QString portInput)
{
    QHostAddress hostAddr(ip_address);

    quint16 port = quint16(portInput.toInt());

    connectToServer(hostAddr, port);
}

void ChatClient::attemptLogin(const QString &username, const QString &password)
{
    login(username, password);
}

int ChatClient::getCurrentStatus()
{
    return m_currentStatus;
}

//void ChatClient::loginFailed(const QString &reason)
//{
//    qDebug()<<tr("Error") + reason;
//
//}

void ChatClient::connectToServer(const QHostAddress &address, quint16 port)
{
    m_clientSocket->connectToHost(address, port);
    qDebug()<<"connexion to server";
}


void ChatClient::login(const QString &userName, const QString &password)
{
    if (m_clientSocket->state() == QAbstractSocket::ConnectedState) { // if the client is connected
        // create a QDataStream operating on the socket
        QDataStream clientStream(m_clientSocket);
        // set the version so that programs compiled with different versions of Qt can agree on how to serialise
        clientStream.setVersion(QDataStream::Qt_5_15);
        // Create the JSON we want to send
        QJsonObject message;
        message["type"] = QStringLiteral("login");
        message["username"] = userName;

        //TODO : rajouter password en content du message;
        message["text"] = password;
        // send the JSON using QDataStream
        clientStream << QJsonDocument(message).toJson();
    }
}


void ChatClient::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}

void ChatClient::sendMessage(const QString &text)
{
    if (text.isEmpty())
        return; // We don't send empty messages
    // create a QDataStream operating on the socket
    QDataStream clientStream(m_clientSocket);
    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
    clientStream.setVersion(QDataStream::Qt_5_15);
    // Create the JSON we want to send
    QJsonObject message;
    message["type"] = QStringLiteral("message");
    message["text"] = text;
    // send the JSON using QDataStream
    clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
}

void ChatClient::jsonReceived(const QJsonObject &docObj)
{
    // actions depend on the type of message
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
        return; // a message with no type was received so we just ignore it


//    //this the first type of connection that we should receive
//    if (typeVal.toString().compare(QLatin1String("newconnection"), Qt::CaseInsensitive) == 0) { //It's a login message
//        if (m_loggedIn)
//            return; // if we are already logged in we ignore
//        // the success field will contain the result of our attempt to login
//        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
//        if (resultVal.isNull() || !resultVal.isBool())
//            return; // the message had no success field so we ignore
//        const bool connectionSuccess = resultVal.toBool();
//        if (connectionSuccess) {
//            // we successfully connected to the server
//            m_currentStatus = notloggedin;

//            emit connected();
//            return;
//        }
//    }


    if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) == 0) { //It's a login message
        if (m_loggedIn)
            return; // if we are already logged in we ignore
        // the success field will contain the result of our attempt to login
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
            return; // the message had no success field so we ignore
        const bool loginSuccess = resultVal.toBool();
        if (loginSuccess) {
            // we logged in successfully and we notify it via the loggedIn signal
            emit loggedIn();
            qDebug()<<"successfull logged in";
            return;
        }
        // the login attempt failed, we extract the reason of the failure from the JSON
        // and notify it via the loginError signal
        const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
        emit loginError(reasonVal.toString());

    } else if (typeVal.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) == 0) { //It's a chat message
        // we extract the text field containing the chat text
        const QJsonValue textVal = docObj.value(QLatin1String("text"));
        // we extract the sender field containing the username of the sender
        const QJsonValue senderVal = docObj.value(QLatin1String("sender"));
        if (textVal.isNull() || !textVal.isString())
            return; // the text field was invalid so we ignore
        if (senderVal.isNull() || !senderVal.isString())
            return; // the sender field was invalid so we ignore
        // we notify a new message was received via the messageReceived signal

        emit messageReceived(senderVal.toString(), textVal.toString());
    } else if (typeVal.toString().compare(QLatin1String("newuser"), Qt::CaseInsensitive) == 0) { // A user joined the chat
        // we extract the username of the new user
        const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return; // the username was invalid so we ignore
        // we notify of the new user via the userJoined signal
        emit userJoined(usernameVal.toString());
    } else if (typeVal.toString().compare(QLatin1String("userdisconnected"), Qt::CaseInsensitive) == 0) { // A user left the chat
         // we extract the username of the new user
        const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return; // the username was invalid so we ignore
        // we notify of the user disconnection the userLeft signal
        emit userLeft(usernameVal.toString());
    }
}

void ChatClient::onReadyRead()
{
    // prepare a container to hold the UTF-8 encoded JSON we receive from the socket
    QByteArray jsonData;
    // create a QDataStream operating on the socket
    QDataStream socketStream(m_clientSocket);
    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
    socketStream.setVersion(QDataStream::Qt_5_15);
    // start an infinite loop
    for (;;) {
        // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
        socketStream.startTransaction();
        // we try to read the JSON data
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            // we successfully read some data
            // we now need to make sure it's in fact a valid JSON
            QJsonParseError parseError;
            // we try to create a json document with the data we received
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                // if the data was indeed valid JSON
                if (jsonDoc.isObject()) // and is a JSON object
                    jsonReceived(jsonDoc.object()); // parse the JSON
            }
            // loop and try to read more JSONs if they are available
        } else {
            // the read failed, the socket goes automatically back to the state it was in before the transaction started
            // we just exit the loop and wait for more data to become available
            break;
        }
    }
}

int ChatClient::currentStatus() const
{
    return m_currentStatus;
}

void ChatClient::setCurrentStatus(int newCurrentStatus)
{
    if (m_currentStatus == newCurrentStatus)
        return;
    m_currentStatus = newCurrentStatus;
    emit currentStatusChanged();
}
