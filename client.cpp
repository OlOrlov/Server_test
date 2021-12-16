#include "client.h"

Client::Client(QHostAddress clientIP_inp, quint16 clientPort_inp,
               QHostAddress serverIP_inp, QString login_inp)
    : clientIP(clientIP_inp), clientPort(clientPort_inp),
      serverIP(serverIP_inp), login(login_inp)
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
    sentHistory.append(sentMsg{sendTime, msgString});

    return true;
}

QByteArray Client::tryReceive(qint64 timeout)
{
    QUdpSocket rcv_sock;
    if ( !rcv_sock.bind(QHostAddress(clientIP), clientPort))
    {
        qDebug() << "Failed to bind rcv socket" << rcv_sock.errorString();
        return QByteArray("");
    }

    tmr.restart();

    while (tmr.elapsed() < timeout)
        if (rcv_sock.hasPendingDatagrams())
        {
            QByteArray received;
            received.resize(rcv_sock.pendingDatagramSize());
            rcv_sock.readDatagram(received.data(), received.size());
            return received;
        }
    qDebug()<<"No message received";
    return QByteArray("");
}

bool Client::authorize()
{
    token.resize(4);
    QUdpSocket xmt_sock;
    QUdpSocket rcv_sock;

    auto msg = (authWord + login).toUtf8();
    if ( !xmt_sock.bind(QHostAddress(clientIP), clientPort))
    {
        qDebug() << "Failed to bind rcv socket" << xmt_sock.errorString();
        return false;
    }

    QElapsedTimer tmrr;
    QByteArray received;
    received.resize(4);

    xmt_sock.connectToHost(serverIP, portForAuthorization);
    xmt_sock.waitForConnected(1);

    tmrr.start();

    auto qha = QHostAddress(clientIP);
    xmt_sock.write(msg);
    xmt_sock.close();

    rcv_sock.bind(qha, clientPort);

    while (true) {
        if (rcv_sock.hasPendingDatagrams())
        {
            rcv_sock.readDatagram(token.data(), 4);
            return true;
        }

        if (tmrr.elapsed() > 2000)
            break;

    }

    return false;
}
