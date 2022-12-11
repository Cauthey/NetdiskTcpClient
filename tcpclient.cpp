#include "tcpclient.h"
#include "ui_tcpclient.h"
#include<QByteArray>
#include<QDebug>
#include<QMessageBox>
#include<QHostAddress>

TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();

    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));

    // 连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);


}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig(){
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        qDebug() << strData;
        file.close();
        strData.replace("\r\n"," ");
        qDebug() << strData ;
        QStringList strList = strData.split(" ");
        qDebug() << strList ;
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << m_strIP ;
        qDebug() << m_usPort ;

    }else{
        QMessageBox::critical(this,"open config","open config failed");
    }
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}
