#include "client.h"

Client::Client(QHostAddress clientIP_inp, quint16 clientPort_inp,
               QHostAddress serverIP_inp)
    : clientIP(clientIP_inp), clientPort(clientPort_inp),
      serverIP(serverIP_inp)
{
}

bool Client::send(QByteArray msg, quint16 port, QString msgString)
{
    QUdpSocket xmt_sock;
    if ( !xmt_sock.bind(QHostAddress(clientIP), clientPort))
    {
        qDebug() << xmt_sock.errorString();
        return false;
    }

    xmt_sock.connectToHost(serverIP, port);
    if ( !xmt_sock.waitForConnected(1))
    {
        qDebug() << "UDP connection to server timeout";
        return false;
    }

    QTime sendTime;

    qint64 r1 = xmt_sock.write(msg);
    if (r1 != msg.length())
    {
        qDebug() << "Client to server msg send failure" << xmt_sock.errorString();
        return false;
    }

    sendTime = QTime::currentTime();
    if (port == portForLogRecord)
        sentHistory.append(sentMsg{sendTime, msgString});

    return true;
}

QByteArray Client::tryReceive(qint64 timeout)
{
    QByteArray received;
    QUdpSocket rcv_sock;
    if ( !rcv_sock.bind(QHostAddress(clientIP), clientPort))
    {
        qDebug() << "Failed to bind rcv socket" << rcv_sock.errorString();
        return received;
    }

    QElapsedTimer tmr;
    tmr.start();
    while (tmr.elapsed() < timeout)
        if (rcv_sock.hasPendingDatagrams())
        {
            received.resize(rcv_sock.pendingDatagramSize());
            rcv_sock.readDatagram(received.data(), received.size());

            //ПРОВЕРКА СООБЩЕНИЯ ОБ ОШИБКЕ
            return received;
        }

    return received;
}
