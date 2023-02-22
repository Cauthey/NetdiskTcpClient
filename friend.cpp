#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include<QInputdialog.h>
#include"privatechat.h"
#include<QMessageBox>


Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE = new QTextEdit;
    m_pFriendListWidget = new QListWidget;
    m_pInputMsgLE = new QLineEdit;
    m_pDelFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友");
    m_pShowOnlineUsrPB = new QPushButton("显示在线");
    m_pSearchUsrPB = new QPushButton("查找用户");
    m_pMsgSendPB = new QPushButton("发送信息");
    m_pPrivateChatPB = new QPushButton("私聊");

    QVBoxLayout *pRightPBVL = new QVBoxLayout;
    pRightPBVL->addWidget(m_pDelFriendPB);
    pRightPBVL->addWidget(m_pFlushFriendPB);
    pRightPBVL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVL->addWidget(m_pSearchUsrPB);
    pRightPBVL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);

    m_pOnline->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(showOnline()));

    connect(m_pSearchUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(searchUser()));
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool))
            ,this,SLOT(flushFriend()));
    connect(m_pDelFriendPB,SIGNAL(clicked(bool))
            ,this,SLOT(delFriend()));
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool))
            ,this,SLOT(privateChat()));

#include<QDebug>

}

void Friend::showAllOnlineUser(PDU *pdu)
{
    if(NULL == pdu){
        return ;
    }
    m_pOnline->showUser(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(NULL == pdu){
        return ;
    }else{
        m_pFriendListWidget->clear();    // 清空状态
    }
    uint uiSize = pdu->uiMsgLen/32;
    char caName[32] = {'\0'};
    for(uint i=0;i<uiSize;i++){
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
        m_pFriendListWidget->addItem(caName);
        qDebug() << caName;
    }
    return ;
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden()){
        m_pOnline->show();
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        m_pOnline->hide();
    }
}

void Friend::searchUser()
{
    m_strSearchName = QInputDialog::getText(this,"搜索","用户名：");
    if(!m_strSearchName.isEmpty()){
        qDebug() << m_strSearchName;
        PDU *pdu = mkPDU(0);
        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Friend::flushFriend()
{
    QString strName = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FRIEND_FLUSH_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::delFriend()
{
    if(NULL != m_pFriendListWidget->currentItem()){
        QString strFriendName =  m_pFriendListWidget->currentItem()->text();
        qDebug() << strFriendName ;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        QString selfName = TcpClient::getInstance().loginName();
        memcpy(pdu->caData,selfName.toStdString().c_str(),selfName.size());
        memcpy(pdu->caData+32,strFriendName.toStdString().c_str(),strFriendName.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Friend::privateChat()
{
    if(NULL != m_pFriendListWidget->currentItem()){
        QString strChatName =  m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().setChatName(strChatName);
        if(PrivateChat::getInstance().isHidden()){
            PrivateChat::getInstance().show();
        }
    }else{
        QMessageBox::warning(this,"私聊","请选择私聊对象");
    }
}
