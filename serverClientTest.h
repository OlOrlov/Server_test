#ifndef SERVERCLIENTTEST_H
#define SERVERCLIENTTEST_H

#include <QObject>
#include <QTest>
#include <QUdpSocket>
#include <QElapsedTimer>
#include <iostream>
#include <vector>
#include <memory>

#include "hcommon.h"
#include "client.h"

class Client;

class ServerClientTest : public QObject
{
    Q_OBJECT
public:
    explicit ServerClientTest(QString logFileName_inp,
                              QString serverIP_inp,
                              QString clientIP_base_inp);

private:
    std::unique_ptr<Client> pFirstClient;
    std::vector<Client> clientVector;
    quint16 clientPort_base = 1100;
    QString clientLogin_base = "Client";
    QByteArray clientToken_base;
    QString clientIP_base;
    QString serverIP;
    QString logFileName;
    QFile logFile;
	
	QStringList msgsList {"alpha", "betha", "gamma", 
						  "epsilon", "zeta", "eta", 
                          "theta", "iota", "kappa"};
	
    bool findLogLine(QString searchLine);
    int getLag(QTime sendTime, QString searchLine);

private slots:
    void initTestCase();
    void authorizationFailTest_1();
    void authorizationFailTest_2();
    void authorizationFailTest_3();
    void authorizationTest();
    void incorrectMsgStructureTest();
    void incorrectLoginTest();
    void incorrectTokenTest();
    void msgRecordTest();
    void stressTest();
    void loadTest();
    void cleanupTestCase();
};

#endif // SERVERCLIENTTEST_H
