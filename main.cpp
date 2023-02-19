#include "tcpclient.h"
#include <QApplication>
//#include "online.h"
//#include"friend.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    TcpClient w;
//    w.show();
    TcpClient::getInstance().show();

//    Friend w;
//    w.show();

//    online w;
//    w.show();

    return a.exec();
}
