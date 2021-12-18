#ifndef CLIENT_H
#define CLIENT_H

#include <QUdpSocket>
#include <QTime>
#include <QElapsedTimer>
#include <thread>
#include <QThread>
#include <condition_variable>
#include <mutex>
#include "hcommon.h"

struct sentMsg{
    QTime time;
    QString message;
};

class Client
{
public:
    Client(QHostAddress clientIP_inp, quint16 clientPort_inp,
           QHostAddress serverIP_inp, QString login_inp);
    bool send(QByteArray msg, quint16 port, QString msgString = "");
    QByteArray tryReceive(qint64 timeout);
    QList<sentMsg> showSentHistory(){return sentHistory;}
	QString getLogin(){return login;}
    QByteArray getToken(){return token;}
    bool authorize();

private:
    QHostAddress clientIP;
    quint16 clientPort;
    QHostAddress serverIP;
	QString login;
    QList<sentMsg> sentHistory;
    QByteArray token;
    QElapsedTimer tmr;
};

#endif // CLIENT_H
