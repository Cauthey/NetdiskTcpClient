#include "tcpclient.h"
#include "ui_tcpclient.h"
#include<QByteArray>
#include<QDebug>
#include<QMessageBox>
#include<QHostAddress>
#include"protocol.h"
TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);

    resize(600,400);

    loadConfig();

    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));


    // 连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);

    // 测试提交
     // 测试提交


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

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

void TcpClient::recvMsg()
{
    qDebug() << m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_RESPOND:
    {
        if(0 == strcmp(pdu->caData,REGIST_SUCCESS)){
            QMessageBox::information(this,"注册",REGIST_SUCCESS);
        }else if(0 == strcmp(pdu->caData,REGIST_FAILED)){
            QMessageBox::warning(this,"注册",REGIST_FAILED);
        }else{
            QMessageBox::warning(this,"注册","unknown error");
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:
    {
        if(0 == strcmp(pdu->caData,LOGIN_SUCCESS)){
            QMessageBox::information(this,"登录",LOGIN_SUCCESS);
            OpeWidget::getInstance().show();
            this->hide();
            //hide(); 隐藏的形参可写可不写this
        }else if(0 == strcmp(pdu->caData,LOGIN_FAILED)){
            QMessageBox::warning(this,"登录",LOGIN_FAILED);
        }else{
            QMessageBox::warning(this,"登录","unknown error");
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->showAllOnlineUser(pdu);

        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu = NULL;
}
#if 0
void TcpClient::on_send_pb_clicked()
{
    QString strMsg = ui->lineEdit->text();
    if (!strMsg.isEmpty()){
        PDU *pdu = mkPDU(strMsg.size());
        pdu->uiMsgType = 8888;
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        QMessageBox::warning(this,"信息发送","发送信息不能为空！");
    }
}
#endif
void TcpClient::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty()&& !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);   // 数组前32位放用户名
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32); // 数组后32位放密码
        qDebug() << pdu->caData;
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::critical(this,"登录","登陆失败，用户名或用户密码为空！");
    }
}

void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty()&& !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);   // 数组前32位放用户名
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32); // 数组后32位放密码
        qDebug() << pdu->caData;
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::critical(this,"注册","注册失败，用户名或用户密码为空！");
    }

}

void TcpClient::on_cancel_pb_clicked()
{

}
