#include "tcpclient.h"
#include <QApplication>
//#include "online.h"
//#include"friend.h"
//#include"book.h"


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

//    Book book;
//    book.show();
    return a.exec();
}
