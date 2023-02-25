#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "opewidget.h"

namespace Ui {
class TcpClient;
}

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = 0);
    ~TcpClient();
    void loadConfig();

    static TcpClient &getInstance();

    QTcpSocket &getTcpSocket();

    QString loginName();
    QString curPath();


public slots:
    void showConnect();
    void recvMsg();
private slots:
//    void on_send_pb_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;

    // 连接服务器和服务器斤西瓜交互
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;

    QString m_strCurPath;
};

#endif // TCPCLIENT_H
