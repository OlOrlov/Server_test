#include <QCoreApplication>
#include <QTest>

#include "serverclientconnection.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString clientIP("127.0.1.1");
    QString serverIP("127.0.0.1");

    qputenv("QTEST_FUNCTION_TIMEOUT", "240000");
    QTest::qExec(new ServerClientTest(serverIP, clientIP), argc, argv);

    return a.exec();
}
