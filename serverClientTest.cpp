#include "serverClientTest.h"

ServerClientTest::ServerClientTest(QString logFileName_inp,
                                   QString serverIP_inp,
                                   QString clientIP_base_inp)
{
    logFileName = logFileName_inp;
    clientIP_base = clientIP_base_inp;
    serverIP = serverIP_inp;
}

bool ServerClientTest::findLogLine(QString searchLine)
{
    QTextStream logStream (&logFile);
    QString line;
    do {
        line = logStream.readLine();

        if (line.mid(13) == searchLine)
            return true;
    }
    while (!line.isNull());

    return false;
}

int ServerClientTest::getLag(QTime sendTime, QString searchLine)
{
    QTextStream logStream (&logFile);
    QString line;
    do {
        line = logStream.readLine();

        if (line.mid(13) == searchLine)
        {
            auto recordTime = QTime::fromString(line.left(12), "hh.mm.ss.zzz");
            return abs(sendTime.msecsTo(recordTime));
        }
    }
    while (!line.isNull());

    return -1;
}

void ServerClientTest::initTestCase()
{
    logFile.setFileName(logFileName);
    QVERIFY2(logFile.open(QIODevice::ReadOnly), "Can't open log file");
    logFile.close();

    Client client(QHostAddress(clientIP_base),
                  clientPort_base,
                  QHostAddress(serverIP),
				  clientLogin_base);
    pFirstClient = std::make_unique<Client>(client);
}

void ServerClientTest::failedAuthorizationTest_1()
{
    printf("\n");

    QByteArray msg;
    msg.append(authWord.toUtf8());
    //No login send - incorrect authorization form

    pFirstClient->send(msg, portForAuthorization);
    auto received = pFirstClient->tryReceive(1500);

    QVERIFY2(received.isEmpty() == true, "Server shouldn't have sent token"
                                         " - incorrect authorization form was sent");
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

    QVERIFY2(received.isEmpty() == true, "Server shouldn't have sent token"
                                         " - incorrect authorization form was sent");
}

void ServerClientTest::failedAuthorizationTest_3()
{
    printf("\n");

    QByteArray msg;
    msg.append("[Au!th]");
    msg.append(clientLogin_base.toUtf8());

    pFirstClient->send(msg, portForAuthorization);

    auto received = pFirstClient->tryReceive(1500);

    QVERIFY2(received.isEmpty() == true, "Server shouldn't have sent token"
                                         " - incorrect authorization form was sent");
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
    QVERIFY2(findLogLine(sent.message) == false, "Server have recorded the message"
                                                 " - incorrect message form was sent");
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
    QVERIFY2(findLogLine(sent.message) == false, "Server have recorded the message"
                                                 " - incorrect message form was sent");
    logFile.close();
}

void ServerClientTest::incorrectTokenTest()
{
    printf("\n");

    QString msgString("test_msg_3");
    QString wrongToken{"481516234"};

    QByteArray msg;
    msg.append(loginWord.toUtf8());
    msg.append(clientLogin_base.toUtf8());
    msg.append(tokenWord.toUtf8());
    msg.append(wrongToken);
    msg.append(msgWord.toUtf8());
    msg.append(msgString.toUtf8());

    pFirstClient->send(msg, portForLogRecord, msgString);
    auto received = pFirstClient->tryReceive(1500);

    QVERIFY2(received.isEmpty() == false, "Server didn't sent error message"
                                          " - incorrect token was sent");

    QVERIFY2(logFile.open(QIODevice::ReadOnly), "Failed to open log");
    auto sent = pFirstClient->showSentHistory().last();
    QVERIFY2(findLogLine(sent.message) == false, "Server have recorded the message"
                                                 " - incorrect message form was sent");
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
    QVERIFY2(findLogLine(sent.message), "Server haven't recorded the message"
                                        " - incorrect message form was sent");
    logFile.close();
}

void ServerClientTest::stressTest()
{
    printf("\n");
	
    auto clientIP = QHostAddress(clientIP_base).toIPv4Address();// Just to ease the address manipulations
    auto clientPort = clientPort_base;
    QString login;

    // Making test clients
    for (int i = 0; i < maxConnectsForStressTest; i++)
    {
        login = clientLogin_base + QString::number(i);
        clientIP++ ;
        clientPort++;
        Client client(QHostAddress(clientIP),
                      clientPort,
                      QHostAddress(serverIP),
                      login);
        clientVector.push_back(client);
    }
	
    // Getting tokens for test clients
    for (int i = 0; i < maxConnectsForStressTest; i++)
    {
        auto authorizationResult = clientVector[i].authorize();
        QVERIFY2(authorizationResult, "Failed to obtain token");
    }

    // Sending test messages
    for (auto msg : msgsList)
    {
        for (int i = 0; i < maxConnectsForStressTest; i++)
        {
            QString msgString = msg + QString::number(i);
            QByteArray toSend;
            toSend.append(loginWord.toUtf8());
            toSend.append(clientVector[i].getLogin().toUtf8());
            toSend.append(tokenWord.toUtf8());
            toSend.append(clientVector[i].getToken());
            toSend.append(msgWord.toUtf8());
            toSend.append(msgString.toUtf8());
            clientVector[i].send(toSend, portForLogRecord, msgString);
        }
    }

    QTest::qWait(1500);
    printf("ok\n");

    std::vector<int> lagReceived;
    for (int i = 0; i < maxConnectsForStressTest; i++)
    {
        for (auto sentMsg : clientVector[i].showSentHistory())
        {
            QVERIFY2(logFile.open(QIODevice::ReadOnly), "Failed to open log");
            auto lag = getLag(sentMsg.time, sentMsg.message);

            if (lag < 0)
                qDebug() << sentMsg.message;
            QVERIFY2(lag >= 0, "Server didn't recorded message");
            lagReceived.push_back(lag);
            logFile.close();
        }
    }

    int meanLag = 0;
    for (auto lag : lagReceived)
        meanLag += lag;

    meanLag /= maxConnectsForStressTest;
    printf("Mean lag for 75 simultaneous connections is %d ms\n\n", meanLag);
}

void ServerClientTest::loadTest()
{
    auto clientIP = QHostAddress(clientIP_base).toIPv4Address();// Just for the easier manipulations
    auto clientPort = clientPort_base;
    QString login;

    quint16 clientsInVector = clientVector.size();

    for (int i = clientsInVector - 1; i < maxConnectsForLoadTest; i++)
    {
        login = clientLogin_base + QString::number(i);
        clientIP++ ;
        clientPort++;
        Client client(QHostAddress(clientIP),
                      clientPort,
                      QHostAddress(serverIP),
                      login);
        clientVector.push_back(client);
    }

    for (int i = 0; i < maxConnectsForLoadTest; i++)
    {
        auto authorizationResult = clientVector[i].authorize();
        if ( !authorizationResult)
        {
            if (i < maxServerConnects-1)
                QFAIL("Failed to obtain token");
            else
                break;
        }
    }
}

void ServerClientTest::cleanupTestCase()
{
    printf("\n");
}
