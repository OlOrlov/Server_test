#ifndef SERVERCLIENTCONNECTION_H
#define SERVERCLIENTCONNECTION_H

#include <QObject>
#include <QTest>
#include <QUdpSocket>
#include <QElapsedTimer>
#include <iostream>

const quint16 portForAuthorization = 8000;
const quint16 portForMessages = 8001;
const QString loginWord = "[Login]";
const QString tokenWord = "[Token]";
const QString msgWord   = "[Msg]";

class ServerClientTest : public QObject
{
    Q_OBJECT
public:
    explicit ServerClientTest(QString serverIP_inp, QString clientIP_base_inp);

private:
    quint16 clientPort_base = 1100;
    QString clientLogin_base = "Client";
    QByteArray clientToken_base;
    QString clientIP_base;
    QString serverIP;
    void incorrectMsgStructureTest();
    void incorrectLoginTest();
    void incorrectTokenTest();
    void stressTest();
    void loadTest();

private slots:
    void initTestCase();
    void authorizationTest();
    void msgRecordTest();
    void cleanupTestCase();
};

#endif // SERVERCLIENTCONNECTION_H
