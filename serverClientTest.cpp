#include "serverclientconnection.h"

ServerClientTest::ServerClientTest(QString logFileName_inp,
                                   QString serverIP_inp,
                                   QString clientIP_base_inp)
{
    logFileName = logFileName_inp;
    clientIP_base = clientIP_base_inp;
    serverIP = serverIP_inp;
}

bool ServerClientTest::findLogLine(QTime sendTime, QString searchLine)
{
    QTextStream logStream (&logFile);
    QString line;
    do {
        line = logStream.readLine();
        if (line.contains(searchLine, Qt::CaseSensitive))
        {
            auto recordTime = QTime::fromString(line.left(8), "hh.mm.ss");
            printf("Found record. Lag: %d ms\n", recordTime.msecsTo(sendTime));
            return true;
        }
    }
    while (!line.isNull());

    return false;
}

void ServerClientTest::initTestCase()
{
    logFile.setFileName(logFileName);
    QVERIFY2(logFile.open(QIODevice::ReadOnly), "Failed to open log");
    logFile.close();

    Client client(QHostAddress(clientIP_base),
                  clientPort_base,
                  QHostAddress(serverIP));
    pFirstClient = std::make_unique<Client>(client);
//    pFirstClient = std::make_unique<Client>(new Client(QHostAddress(clientIP_base),
//                                                       clientPort_base,
//                                                       QHostAddress(serverIP)));
}

void ServerClientTest::failedAuthorizationTest_1()
{
    printf("\n");

    QByteArray msg;
    msg.append(authWord.toUtf8());
    //No login send

    pFirstClient->send(msg, portForAuthorization);

    auto received = pFirstClient->tryReceive(1500);

    QVERIFY2(received.isEmpty() == true, "Server shouldn't have sent token");
}

void ServerClientTest::failedAuthorizationTest_2()
{
    printf("\n");

    QByteArray msg;
    msg.append(authWord.toUtf8());
    QByteArray oversizedLogin;
    oversizedLogin.fill(0xAA, maxLoginSize + authWordLength + 1);
    msg.append(oversizedLogin);

    pFirstClient->send(msg, portForAuthorization);

    auto received = pFirstClient->tryReceive(1500);

    QVERIFY2(received.isEmpty() == true, "Server shouldn't have sent token");
}

void ServerClientTest::failedAuthorizationTest_3()
{
    printf("\n");

    QByteArray msg;
    msg.append("[Au!th]");
    msg.append(clientLogin_base.toUtf8());

    pFirstClient->send(msg, portForAuthorization);

    auto received = pFirstClient->tryReceive(1500);

    QVERIFY2(received.isEmpty() == true, "Server shouldn't have sent token");
}

void ServerClientTest::authorizationTest()
{
    printf("\n");

    QByteArray msg;
    msg.append(authWord.toUtf8());
    msg.append(clientLogin_base);

    pFirstClient->send(msg, portForAuthorization);

    clientToken_base.append(pFirstClient->tryReceive(1500));

    QVERIFY2(clientToken_base.isEmpty() == false, "Server didn't sent token");
}

void ServerClientTest::incorrectMsgStructureTest()
{
    printf("\n");

    QString msgString("test_msg_1");
    QString wrongTokenWord("[!Token]");

    QByteArray msg;
    msg.append(loginWord.toUtf8());
    msg.append(clientLogin_base.toUtf8());
    msg.append(wrongTokenWord.toUtf8());
    msg.append(clientToken_base);
    msg.append(msgWord.toUtf8());
    msg.append(msgString.toUtf8());

    pFirstClient->send(msg, portForLogRecord, msgString);

    QVERIFY2(logFile.open(QIODevice::ReadOnly), "Failed to open log");

    auto sent = pFirstClient->showSentHistory().last();
    QVERIFY2(findLogLine(sent.time, sent.message) == false, "Server have recorded the message");
    logFile.close();
}

void ServerClientTest::incorrectLoginTest()
{
    printf("\n");

    QString msgString("test_msg_2");
    QString wrongloginWord("[Clien!t]");
    QByteArray msg;
    msg.append(wrongloginWord.toUtf8());
    msg.append(clientLogin_base.toUtf8());
    msg.append(tokenWord.toUtf8());
    msg.append(clientToken_base);
    msg.append(msgWord.toUtf8());
    msg.append(msgString.toUtf8());

    pFirstClient->send(msg, portForLogRecord, msgString);

    QVERIFY2(logFile.open(QIODevice::ReadOnly), "Failed to open log");
    auto sent = pFirstClient->showSentHistory().last();
    QVERIFY2(findLogLine(sent.time, sent.message) == false, "Server have recorded the message");
    logFile.close();
}

void ServerClientTest::incorrectTokenTest()
{
    printf("\n");

    QString msgString("test_msg_3");
    QString wrongToken{"815162342"};

    QByteArray msg;
    msg.append(loginWord.toUtf8());
    msg.append(clientLogin_base.toUtf8());
    msg.append(tokenWord.toUtf8());
    msg.append(wrongToken);
    msg.append(msgWord.toUtf8());
    msg.append(msgString.toUtf8());

    pFirstClient->send(msg, portForLogRecord, msgString);
    auto received = pFirstClient->tryReceive(1500);

    QVERIFY2(received.isEmpty() == false, "Server didn't sent error message");

    QVERIFY2(logFile.open(QIODevice::ReadOnly), "Failed to open log");
    auto sent = pFirstClient->showSentHistory().last();
    QVERIFY2(findLogLine(sent.time, sent.message) == false, "Server have recorded the message");
    logFile.close();
}

void ServerClientTest::msgRecordTest()
{
    printf("\n");

    QString msgString("test_msg_4");

    QByteArray msg;
    msg.append(loginWord.toUtf8());
    msg.append(clientLogin_base.toUtf8());
    msg.append(tokenWord.toUtf8());
    msg.append(clientToken_base);
    msg.append(msgWord.toUtf8());
    msg.append(msgString.toUtf8());

    pFirstClient->send(msg, portForLogRecord, msgString);
    auto received = pFirstClient->tryReceive(1500);

    QVERIFY2(received.isEmpty() == true, "Server sent error message");

    QVERIFY2(logFile.open(QIODevice::ReadOnly), "Failed to open log");
    auto sent = pFirstClient->showSentHistory().last();
    QVERIFY2(findLogLine(sent.time, sent.message), "Server haven't recorded the message");
    logFile.close();
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
