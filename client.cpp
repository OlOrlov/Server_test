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
    return QByteArray("");
}

bool Client::authorize()
{
    token.resize(4);
    bool tokenReceived = false;

    QUdpSocket xmt_sock;

    auto msg = (authWord + login).toUtf8();
    if ( !xmt_sock.bind(QHostAddress(clientIP), clientPort, QUdpSocket::ReuseAddressHint))
    {
        qDebug() << "Failed to bind rcv socket" << xmt_sock.errorString();
        return false;
    }

    std::mutex mtxForCV;
    std::unique_lock<std::mutex> lockForCV(mtxForCV);
    std::condition_variable waitUntilReceiving;

    std::thread rcvThread([&] () mutable {
        QUdpSocket rcv_sock;
        if ( !rcv_sock.bind(QHostAddress(clientIP), clientPort, QUdpSocket::ReuseAddressHint))
        {
            qDebug() << "Failed to bind rcv socket" << rcv_sock.errorString();
        }

        QElapsedTimer tmrr;
        tmrr.start();
        waitUntilReceiving.notify_one();

        while (!tokenReceived)
        {
            if (rcv_sock.hasPendingDatagrams())
            {
                rcv_sock.readDatagram(token.data(), 4);
                tokenReceived = true;
            }

            if (tmrr.elapsed() > 3000)
                break;
        }
    });

    waitUntilReceiving.wait(lockForCV);

    xmt_sock.connectToHost(serverIP, portForAuthorization);
    xmt_sock.waitForConnected(1);
    xmt_sock.write(msg);
    xmt_sock.close();

    rcvThread.join();

    return tokenReceived;
}
