#include <QCoreApplication>
#include <QTest>

#include "serverClientTest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString clientIP("127.0.1.1");
    QString serverIP("127.0.0.1");

    printf("\nTurn server on localhost and then enter full address to it's Log.txt:\n");

    QTextStream s(stdin);
    QString logFileName = s.readLine();

    qputenv("QTEST_FUNCTION_TIMEOUT", "240000");
    QTest::qExec(new ServerClientTest(logFileName, serverIP, clientIP), argc, argv);

    return a.exec();
}
