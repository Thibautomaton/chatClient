#ifndef INFODISPLAY_H
#define INFODISPLAY_H

#include <QObject>
#include <chatclient.h>

struct Message{
    Q_GADGET
public:
    Message() : text(""), user("anonymous") {}
    Message(QString textVal, QString senderVal){
        text = textVal;
        user = senderVal;
    }
    QString text;
    QString user;
};
Q_DECLARE_METATYPE(Message)

class InfoDisplay : public QObject
{
    /**
     * Class for updating the values in a message
     * Getters for login infos and messages content
     *
    **/
    Q_OBJECT
public:
    explicit InfoDisplay(QObject *parent = nullptr);

public slots:
    void newMessage( QString senderVal, QString textVal);
    QList<QString> getUsers() { return m_users;};
    QList<Message> getMessages() { return m_messages;};

private:
    QList<Message> m_messages;
    ChatClient *client;
    QList<QString> m_users;
    QString current_login;
};

#endif // INFODISPLAY_H
