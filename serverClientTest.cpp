#include "serverclientconnection.h"

ServerClientTest::ServerClientTest(QString serverIP_inp, QString clientIP_base_inp)
{
    clientIP_base = clientIP_base_inp;
    serverIP = serverIP_inp;
}

void ServerClientTest::initTestCase()
{

}

void ServerClientTest::authorizationTest()
{
    printf("\n");
    QUdpSocket xmt_sock;
    if ( !xmt_sock.bind(QHostAddress(clientIP_base), clientPort_base))
    {
        qDebug() << xmt_sock.errorString();
        QFAIL("Failed to bind rcv socket");
    }

    QByteArray msg;
    msg.append(loginWord.toUtf8());
    msg.append(clientLogin_base);

    xmt_sock.connectToHost(QHostAddress(serverIP), portForAuthorization);
    if ( !xmt_sock.waitForConnected(1))
        QFAIL("UDP connection to server timeout");

    qint64 r1 = xmt_sock.write(msg);
    if (r1 != msg.length())
        QFAIL("Client to server msg send failure");
    xmt_sock.close();

    QUdpSocket rcv_sock;
    if ( !rcv_sock.bind(QHostAddress(clientIP_base), clientPort_base))
    {
        qDebug() << rcv_sock.errorString();
        QFAIL("Failed to bind rcv socket");
    }

    bool tokenReceived = false;
    QElapsedTimer tmr;
    tmr.start();
    while (tmr.elapsed() < 1500)
        if (rcv_sock.hasPendingDatagrams())
        {
            QByteArray received;
            received.resize(rcv_sock.pendingDatagramSize());
            rcv_sock.readDatagram(received.data(), received.size());

            clientToken_base.append(received);
            tokenReceived = true;
            break;
        }
    rcv_sock.close();
    QVERIFY2(tokenReceived, "Server didn't sent token");
}

void ServerClientTest::incorrectMsgStructureTest()
{
    printf("\n");

    QUdpSocket xmt_sock;
    if ( !xmt_sock.bind(QHostAddress(clientIP_base), clientPort_base))
    {
        qDebug() << xmt_sock.errorString();
        QFAIL("Failed to bind rcv socket");
    }

    QString msgString("Hello, Server!");
    QString wrongTokenWord("[!Token]");
    QByteArray msg;
    msg.append(loginWord.toUtf8());
    msg.append(clientLogin_base.toUtf8());
    msg.append(wrongTokenWord.toUtf8());
    msg.append(clientToken_base);
    msg.append(msgWord.toUtf8());
    msg.append(msgString.toUtf8());

    xmt_sock.connectToHost(serverIP, portForMessages);
    if ( !xmt_sock.waitForConnected(1))
        QFAIL("UDP connection to server timeout");

    qint64 r1 = xmt_sock.write(msg);
    if (r1 != msg.length())
        QFAIL("Client to server msg send failure");
    xmt_sock.close();

    QUdpSocket rcv_sock;
    if ( !rcv_sock.bind(QHostAddress(clientIP_base), clientPort_base))
    {
        qDebug() << rcv_sock.errorString();
        QFAIL("Failed to bind rcv socket");
    }
    bool errorMsgReceived = false;
    QElapsedTimer tmr;
    tmr.start();
    while (tmr.elapsed() < 1500)
        if (rcv_sock.hasPendingDatagrams())
        {
            QByteArray received;
            received.resize(rcv_sock.pendingDatagramSize());
            rcv_sock.readDatagram(received.data(), received.size());

            //ПРОВЕРКА СООБЩЕНИЯ ОБ ОШИБКЕ
            errorMsgReceived = true;
            break;
        }
    rcv_sock.close();
    QVERIFY2(errorMsgReceived, "Server didn't sent error message");
}

void ServerClientTest::incorrectLoginTest()
{
    printf("\n");
    QUdpSocket xmt_sock;
    if ( !xmt_sock.bind(QHostAddress(clientIP_base), clientPort_base))
    {
        qDebug() << xmt_sock.errorString();
        QFAIL("Failed to bind rcv socket");
    }

    QString msgString("Hello, Server!");
    QString wrongloginWord("Clien!t");
    QByteArray msg;
    msg.append(wrongloginWord.toUtf8());
    msg.append(clientLogin_base.toUtf8());
    msg.append(tokenWord.toUtf8());
    msg.append(clientToken_base);
    msg.append(msgWord.toUtf8());
    msg.append(msgString.toUtf8());

    xmt_sock.connectToHost(serverIP, portForMessages);
    if ( !xmt_sock.waitForConnected(1))
        QFAIL("UDP connection to server timeout");

    qint64 r1 = xmt_sock.write(msg);
    if (r1 != msg.length())
        QFAIL("Client to server msg send failure");
    xmt_sock.close();

    QUdpSocket rcv_sock;
    if ( !rcv_sock.bind(QHostAddress(clientIP_base), clientPort_base))
    {
        qDebug() << rcv_sock.errorString();
        QFAIL("Failed to bind rcv socket");
    }
    bool errorMsgReceived = false;
    QElapsedTimer tmr;
    tmr.start();
    while (tmr.elapsed() < 1500)
        if (rcv_sock.hasPendingDatagrams())
        {
            QByteArray received;
            received.resize(rcv_sock.pendingDatagramSize());
            rcv_sock.readDatagram(received.data(), received.size());

            //ПРОВЕРКА СООБЩЕНИЯ ОБ ОШИБКЕ
            errorMsgReceived = true;
            break;
        }
    rcv_sock.close();
    //READ LOG FILE!!!!!!!!!!!!!!!!!!!
    QVERIFY2(errorMsgReceived, "Server didn't sent error message");
}

void ServerClientTest::incorrectTokenTest()
{
    printf("\n");
    QUdpSocket xmt_sock;
    if ( !xmt_sock.bind(QHostAddress(clientIP_base), clientPort_base))
    {
        qDebug() << xmt_sock.errorString();
        QFAIL("Failed to bind rcv socket");
    }

    QString msgString("Hello, Server!");
    QByteArray wrongToken{"\x4\x8\x15\x16\x23\x42"};

    QByteArray msg;
    msg.append(loginWord.toUtf8());
    msg.append(clientLogin_base.toUtf8());
    msg.append(tokenWord.toUtf8());
    msg.append(wrongToken);
    msg.append(msgWord.toUtf8());
    msg.append(msgString.toUtf8());

    xmt_sock.connectToHost(serverIP, portForMessages);
    if ( !xmt_sock.waitForConnected(1))
        QFAIL("UDP connection to server timeout");

    qint64 r1 = xmt_sock.write(msg);
    if (r1 != msg.length())
        QFAIL("Client to server msg send failure");
    xmt_sock.close();

    QUdpSocket rcv_sock;
    if ( !rcv_sock.bind(QHostAddress(clientIP_base), clientPort_base))
    {
        qDebug() << rcv_sock.errorString();
        QFAIL("Failed to bind rcv socket");
    }
    bool errorMsgReceived = false;
    QElapsedTimer tmr;
    tmr.start();
    while (tmr.elapsed() < 1500)
        if (rcv_sock.hasPendingDatagrams())
        {
            QByteArray received;
            received.resize(rcv_sock.pendingDatagramSize());
            rcv_sock.readDatagram(received.data(), received.size());

            //ПРОВЕРКА СООБЩЕНИЯ ОБ ОШИБКЕ
            errorMsgReceived = true;
            break;
        }
    rcv_sock.close();
    //READ LOG FILE!!!!!!!!!!!!!!!!!!!
    QVERIFY2(errorMsgReceived, "Server didn't sent error message");
}

void ServerClientTest::msgRecordTest()
{
    printf("\n");
    QUdpSocket xmt_sock;
    xmt_sock.bind(QHostAddress(clientIP_base), clientPort_base);

    QString msgString("Hello, Server!");

    QByteArray msg;
    msg.append(loginWord.toUtf8());
    msg.append(clientLogin_base.toUtf8());
    msg.append(tokenWord.toUtf8());
    msg.append(clientToken_base);
    msg.append(msgWord.toUtf8());
    msg.append(msgString.toUtf8());

    xmt_sock.connectToHost(serverIP, portForMessages);
    if ( !xmt_sock.waitForConnected(1))
        QFAIL("UDP connection to server timeout");

    qint64 r1 = xmt_sock.write(msg);
    if (r1 != msg.length())
        QFAIL("Client to server msg send failure");
    xmt_sock.close();

    //READ LOG FILE
}

void ServerClientTest::stressTest()
{
    printf("\n");
}

void ServerClientTest::loadTest()
{
    printf("\n");
}

void ServerClientTest::cleanupTestCase()
{
    printf("\n");
}
