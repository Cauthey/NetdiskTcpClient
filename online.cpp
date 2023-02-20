#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

online::online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::online)
{
    ui->setupUi(this);
}

online::~online()
{
    delete ui;
}

void online::showUser(PDU *pdu)
{
    if(NULL==pdu){
        return ;
    }
    int uiSize = pdu->uiMsgLen/32;
    char caTemp[32];
    ui->Online_listWidget->clear();
    for(int i=0;i< uiSize;i++){
        memcpy(caTemp,(char*)pdu->caMsg+i*32,32);
        ui->Online_listWidget->addItem(caTemp);
    }
}

void online::on_addFriend_pb_clicked()
{
    QListWidgetItem *pItem =  ui->Online_listWidget->currentItem();
    QString strPerUserName =  pItem->text();
    QString strLoginName = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,strPerUserName.toStdString().c_str(),strPerUserName.size());
    memcpy(pdu->caData+32,strLoginName.toStdString().c_str(),strLoginName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

