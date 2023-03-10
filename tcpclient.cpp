#include "tcpclient.h"
#include "ui_tcpclient.h"
#include<QByteArray>
#include<QDebug>
#include<QMessageBox>
#include<QHostAddress>
#include"protocol.h"
#include"privatechat.h"
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

QString TcpClient::loginName()
{
    return  m_strLoginName;
}

QString TcpClient::curPath()
{
    return m_strCurPath;
}

void TcpClient::setCurPath(QString strCurPath)
{
    m_strCurPath = strCurPath;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

void TcpClient::recvMsg()
{
    if(!OpeWidget::getInstance().getBook()->getDownloadStatus()){
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
                m_strCurPath = QString("./%1").arg(m_strLoginName);
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
        case ENUM_MSG_TYPE_SEARCH_USER_RESPOND:
        {
            if(0==strcmp(SEARCH_USR_NO,pdu->caData)){
                QMessageBox::information(this,"搜索",QString("%1: not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
            }else if (0==strcmp(SEARCH_USR_ONLINE,pdu->caData)){
                QMessageBox::information(this,"搜索",QString("%1: online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
            }else if (0==strcmp(SEARCH_USR_OFFLINE,pdu->caData)){
                QMessageBox::information(this,"搜索",QString("%1: offine").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
            }
            OpeWidget::getInstance().getFriend()->m_strSearchName = "";
            break;
        }
        //  TODO
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char caName[32]={'\0'};
            strncpy(caName,pdu->caData+32,32);
            int res = QMessageBox::information(this,"添加好友",QString("%1 want to add you as friend!").arg(caName)
                                     ,QMessageBox::Yes,QMessageBox::No);
            PDU *respdu = mkPDU(0);
            memcpy(respdu->caData,pdu->caData,32);
            if(QMessageBox::Yes==res){
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
            }else{
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE ;
            }
            m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu =NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
        {
            QMessageBox::information(this,"添加好友",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_FRIEND_FLUSH_RESPOND:
        {
            OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        {
            char caName[32] = {'\0'};
            memcpy(caName,pdu->caData,32);
            QMessageBox::information(this,"删除好友",QString("%1 已将你删除").arg(caName));
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
        {
            QMessageBox::information(this,"删除好友","删除好友成功");
           break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        {
            if(PrivateChat::getInstance().isHidden()){
                PrivateChat::getInstance().show();
            }
            char caSendName[32]={'\0'};
            memcpy(caSendName,pdu->caData,32);
            QString strSendName = caSendName;
            PrivateChat::getInstance().setChatName(strSendName);
            PrivateChat::getInstance().updateMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
        {
            OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:
        {
            QMessageBox::information(this,"创建文件夹",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
        {
            OpeWidget::getInstance().getBook()->clearFileList();
            OpeWidget::getInstance().getBook()->updateFileList(pdu);
            QString strEnterDir = OpeWidget::getInstance().getBook()->getEnterDir();
            if(!strEnterDir.isEmpty()){
                m_strCurPath = m_strCurPath + "/"+strEnterDir;
                qDebug() << "enter dir : "  <<  m_strCurPath;
            }
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_RESPOND:
        {
            QMessageBox::information(this,"删除文件夹",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_RENAME_DIR_RESPOND:
        {
            QMessageBox::information(this,"重命名文件",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:
        {
            OpeWidget::getInstance().getBook()->clearEnterDir();
            QMessageBox::information(this,"进入文件夹",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_DEL_FILE_RESPOND:
        {
            QMessageBox::information(this,"删除文件",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
        {
            QMessageBox::information(this,"上传文件",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:
        {
            qDebug() << pdu->caData;
            char caFileName[32] = {'\0'};
            sscanf(pdu->caData,"%s %lld",caFileName,&(OpeWidget::getInstance().getBook()->m_iTotal));
            if(strlen(caFileName)>0 && OpeWidget::getInstance().getBook()->m_iTotal > 0 ){
                OpeWidget::getInstance().getBook()->setDownloadFile(true);
                QString strSaveFilePath = OpeWidget::getInstance().getBook()->getStrSaveFilePath();
                m_file.setFileName(strSaveFilePath);
                if(!m_file.open(QIODevice::WriteOnly)){
                    QMessageBox::warning(this,"下载文件","获取保存文件的路径失败!");
                }
            }
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:
        {
            QMessageBox::information(this,"共享文件",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_INFORM:
        {
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            // strrchr 从后往前找第一个字符
            char *pos = strrchr(pPath,'/');
            if(NULL!=pos){
                pos++;
                QString strNotice = QString("%1 share file->%2\n Do you recived ths file:").arg(pdu->caData).arg(pos);
                int ret = QMessageBox::question(this,"共享文件",strNotice);
                if(QMessageBox::Yes == ret){
                    PDU *respdu =  mkPDU(pdu->uiMsgLen);
                    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
                    memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
                    QString strName = TcpClient::getInstance().loginName();
                    strcpy(respdu->caData,strName.toStdString().c_str());
                    m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
                }

            }

        }
        default:
            break;
        }
        free(pdu);
        pdu = NULL;
    }else{
        QByteArray buffer =  m_tcpSocket.readAll();
        m_file.write(buffer);
        Book *pBook = OpeWidget::getInstance().getBook();
        pBook->m_iRecved += buffer.size();
        if(pBook->m_iTotal==pBook->m_iRecved){
            m_file.close();
            pBook->m_iTotal=0;
            pBook->m_iRecved=0;
            pBook->setDownloadFile(false);
        }
        else if(pBook->m_iTotal<pBook->m_iRecved){
            m_file.close();
            pBook->m_iTotal=0;
            pBook->m_iRecved=0;
            pBook->setDownloadFile(false);

            QMessageBox::critical(this,"下载文件","下载文件失败!");
        }



    }
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
        m_strLoginName = strName;
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
