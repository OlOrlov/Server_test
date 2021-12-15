#include <QCoreApplication>
#include <QTest>

#include "serverclientconnection.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString clientIP("127.0.1.1");
    QString serverIP("127.0.0.1");

    QString logFileName = "C:/Users/Orlov.Oleg/Desktop/Literature/ClientServer/Server/debug/Log.txt";

    qputenv("QTEST_FUNCTION_TIMEOUT", "240000");
    QTest::qExec(new ServerClientTest(logFileName, serverIP, clientIP), argc, argv);

    return a.exec();
}
