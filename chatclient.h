#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <map>


class ChatClient : public QObject
{
    Q_OBJECT



public:


    enum status {
        notConnected = Qt::UserRole,
        notloggedin = Qt::UserRole +1,
        loggedin = Qt::UserRole +2
    };
    explicit ChatClient(QObject *parent = nullptr);
    Q_INVOKABLE void attemptConnection(QString ip, QString port);
    Q_INVOKABLE void attemptLogin(const QString &username, const QString &password);
    Q_INVOKABLE int getCurrentStatus();


    Q_PROPERTY(int currentStatus READ currentStatus WRITE setCurrentStatus NOTIFY currentStatusChanged);

    //void loginFailed(const QString &reason);

    int currentStatus() const;
    void setCurrentStatus(int newCurrentStatus);

public slots:
    void connectToServer(const QHostAddress &address, quint16 port);

    void login(const QString &userName, const QString &password);
    void sendMessage(const QString &text);
    void disconnectFromHost();

private slots:
    void onReadyRead();

signals:
    void connected();
    void loggedIn();
    void loginError(const QString &reason);
    void disconnected();
    void messageReceived(const QString &sender, const QString &text);
    void error(QAbstractSocket::SocketError socketError);
    void userJoined(const QString &username);
    void userLeft(const QString &username);

    void currentStatusChanged();

private:
    QTcpSocket *m_clientSocket;
    bool m_loggedIn;
    void jsonReceived(const QJsonObject &doc);


    int m_currentStatus = loggedin;
};

#endif // CHATCLIENT_H
