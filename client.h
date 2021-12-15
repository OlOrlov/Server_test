#ifndef CLIENT_H
#define CLIENT_H

#include <QUdpSocket>
#include <QTime>
#include <QElapsedTimer>
#include "hcommon.h"

struct sentMsg{
    QTime time;
    QString message;
};

class Client
{
public:
    Client(QHostAddress clientIP_inp, quint16 clientPort_inp,
           QHostAddress serverIP_inp);
    bool send(QByteArray msg, quint16 port, QString msgString = "");
    QByteArray tryReceive(qint64 timeout);
    QList<sentMsg> showSentHistory(){return sentHistory;}

private:
    QHostAddress clientIP;
    quint16 clientPort;
    QHostAddress serverIP;
    QList<sentMsg> sentHistory;
};

#endif // CLIENT_H
